// Copyright (c) 2026 Aregtech
'use strict';

const assert = require('node:assert/strict');
const { test } = require('node:test');
const admit = require('./ci-admission.js');

function fixture(snapshots) {
    let calls = 0;
    const messages = [];
    const listJobs = () => {};
    return {
        options: {
            github: {
                rest: { actions: { listJobsForWorkflowRun: listJobs } },
                paginate: async (method, params) => {
                    assert.equal(method, listJobs);
                    assert.deepEqual(params, {
                        owner: 'aregtech', repo: 'areg-sdk', run_id: 42, filter: 'latest', per_page: 100
                    });
                    return snapshots[Math.min(calls++, snapshots.length - 1)];
                }
            },
            context: { repo: { owner: 'aregtech', repo: 'areg-sdk' }, runId: 42 },
            core: { info: message => messages.push(message) },
            priority: [
                { name: 'linux-test', os: 'ubuntu-latest' },
                { name: 'linux-benchmark', os: 'ubuntu-latest' },
                { name: 'windows-test', os: 'windows-latest' }
            ],
            runner: 'ubuntu-latest',
            timeoutMs: 1000,
            intervalMs: 0
        },
        calls: () => calls,
        messages
    };
}

test('admits build-only jobs while test and benchmark jobs are still running', async () => {
    const f = fixture([[
        { name: 'linux-test', status: 'in_progress' },
        { name: 'linux-benchmark', status: 'in_progress' },
        { name: 'windows-test', status: 'queued' }
    ]]);
    await admit(f.options);
    assert.equal(f.calls(), 1);
    assert.match(f.messages.at(-1), /admitting build-only jobs/);
});

test('waits for queued and not-yet-created jobs, regardless of started_at', async () => {
    const f = fixture([
        [{ name: 'linux-test', status: 'queued', started_at: '2026-11-04T23:38:26Z' }],
        [
            { name: 'linux-test', status: 'completed', conclusion: 'success' },
            { name: 'linux-benchmark', status: 'in_progress' }
        ]
    ]);
    await admit(f.options);
    assert.equal(f.calls(), 2);
    assert.match(f.messages[0], /linux-test, linux-benchmark/);
});

test('a failed or cancelled priority job does not suppress the remaining builds', async () => {
    const f = fixture([[
        { name: 'linux-test', status: 'completed', conclusion: 'failure' },
        { name: 'linux-benchmark', status: 'completed', conclusion: 'cancelled' }
    ]]);
    await admit(f.options);
    assert.equal(f.calls(), 1);
});

test('a runner with no priority jobs needs no API request', async () => {
    const f = fixture([]);
    await admit({ ...f.options, runner: 'macos-latest' });
    assert.equal(f.calls(), 0);
});

test('API failures are reported instead of silently admitting builds', async () => {
    const f = fixture([]);
    f.options.github.paginate = async () => { throw new Error('API unavailable'); };
    await assert.rejects(admit(f.options), /API unavailable/);
});

test('a bounded wait reports every priority job that has not started', async () => {
    const f = fixture([[{ name: 'linux-test', status: 'in_progress' }]]);
    await assert.rejects(admit({ ...f.options, timeoutMs: 0 }),
        /Admission timed out waiting for ubuntu-latest: linux-benchmark/);
});
