// Copyright (c) 2026 Aregtech
'use strict';

const { setTimeout: sleep } = require('node:timers/promises');

module.exports = async function admit({
    github, context, core, priority, runner,
    timeoutMs = 60 * 60 * 1000, intervalMs = 15000
}) {
    const expected = priority.filter(config => config.os === runner).map(config => config.name);
    if (expected.length === 0) {
        core.info(`No priority jobs for ${runner}.`);
        return;
    }

    const deadline = Date.now() + timeoutMs;
    let previous = '';
    for (;;) {
        const jobs = await github.paginate(github.rest.actions.listJobsForWorkflowRun, {
            ...context.repo, run_id: context.runId, filter: 'latest', per_page: 100
        });
        const started = new Set(jobs
            .filter(job => job.status === 'in_progress' || job.status === 'completed')
            .map(job => job.name));
        const pending = expected.filter(name => !started.has(name));
        if (pending.length === 0) {
            core.info(`All ${expected.length} priority jobs for ${runner} have started; admitting build-only jobs.`);
            return;
        }
        if (Date.now() >= deadline) {
            throw new Error(`Admission timed out waiting for ${runner}: ${pending.join(', ')}`);
        }
        const message = `Waiting for ${runner} priority jobs to start: ${pending.join(', ')}`;
        if (message !== previous) {
            core.info(message);
            previous = message;
        }
        await sleep(intervalMs);
    }
};
