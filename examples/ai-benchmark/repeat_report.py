#!/usr/bin/env python3
"""What --repeat has spent so far, and the band it ended with.

The ledger is one run directory per line, in the order they ran. `--after N` prints the
line for the run that just finished and the running total; `--band` prints the spread
over every run in the ledger.

Output tokens are printed beside the cost because the share of a plan's limit a run
consumes tracks output, not dollars. The cost is analyze_run.py's "cost, cold @1h", so a
run that began on a warm cache compares with one that did not; without its transcript
the billed cost is printed, marked "billed".
"""

import argparse
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import analyze_run  # noqa: E402
import measure  # noqa: E402


def measured(run):
    """{cost, output, thinking, requests, label} of one run directory, or None."""
    path = os.path.join(run, 'result.json')
    try:
        with open(path, encoding='utf-8') as handle:
            report = json.load(handle)
    except (OSError, ValueError):
        return None
    if measure.copilot_usage(report) is not None:
        return copilot_measured(run, report)
    usage = report.get('usage') or {}
    details = usage.get('output_tokens_details') or {}
    cost = report.get('total_cost_usd')
    cold = analyze_run.cold_cost(run)
    return {
        'label': os.path.basename(run.rstrip(os.sep)),
        'cost': cold[0] if cold else (cost if isinstance(cost, (int, float)) else None),
        'billed': cold is None,
        'bill': cost if isinstance(cost, (int, float)) else None,
        'output': usage.get('output_tokens') or 0,
        'thinking': details.get('thinking_tokens') or 0,
        'requests': cold[1] if cold else None,
        'reason': report.get('terminal_reason') or report.get('stop_reason') or '',
    }


def copilot_measured(run, report):
    """The same row from Copilot's usage file: AI credits converted at measure.AIC_USD."""
    nano = report.get('totalNanoAiu')
    cost = (float(measure.AIC_USD) * nano / 1e9
            if isinstance(nano, int) and not isinstance(nano, bool) else None)
    output = thinking = 0
    requests = None
    for entry in (report.get('modelMetrics') or {}).values():
        usage = entry.get('usage') or {}
        output += usage.get('outputTokens') or 0
        thinking += usage.get('reasoningTokens') or 0
        count = (entry.get('requests') or {}).get('count')
        if isinstance(count, int):
            requests = (requests or 0) + count
    return {
        'label': os.path.basename(run.rstrip(os.sep)),
        'cost': cost,
        'billed': True,
        'bill': cost,
        'output': output,
        'thinking': thinking,
        'requests': requests,
        'reason': '',
    }


def rows_of(ledger):
    """Every run named in the ledger that has a measurement, in order."""
    try:
        with open(ledger, encoding='utf-8') as handle:
            runs = [line.strip() for line in handle if line.strip()]
    except OSError:
        return []
    found = []
    for run in runs:
        row = measured(run)
        if row is not None:
            found.append(row)
    return found


def money(value):
    return 'n/a' if value is None else '${:.3f}'.format(value)


def asked(row):
    """The request count, or n/a when the transcript was not found."""
    return 'n/a' if row['requests'] is None else row['requests']


def mark(row):
    return ' billed' if row['billed'] else ''


def after(rows, index, total):
    """The line printed as soon as one run of a repeat has finished."""
    if not rows:
        print('repeat: run {} of {} left no result.json to measure'.format(index, total))
        return
    last = rows[-1]
    spent = sum(row['bill'] for row in rows if row['bill'] is not None)
    output = sum(row['output'] for row in rows)
    print('repeat: {} of {} done -- {}  {}{}  {} output tok  {} requests{}'
          .format(index, total, last['label'], money(last['cost']), mark(last),
                  '{:,}'.format(last['output']), asked(last),
                  '' if last['reason'] in ('completed', 'end_turn', '')
                  else '  ' + last['reason']))
    print('repeat: billed so far {} over {} run(s), {} output tok'
          .format(money(spent), len(rows), '{:,}'.format(output)))
    if index < total:
        estimate = spent / len(rows) * (total - len(rows))
        print('repeat: {} run(s) left, about {} more at this rate'
              .format(total - index, money(estimate)))


def band(rows, total):
    """The spread over the whole repeat: what a single run could not have told you."""
    print('')
    print('repeat: {} of {} run(s) measured'.format(len(rows), total))
    for row in rows:
        print('   {:<28} {:>8}{}  {:>9} output tok  {:>3} requests'
              .format(row['label'], money(row['cost']), mark(row),
                      '{:,}'.format(row['output']), asked(row)))
    costs = [row['cost'] for row in rows if row['cost'] is not None]
    if not costs:
        print('   no cost was recorded; the band cannot be given')
        return
    low, high = min(costs), max(costs)
    mean = sum(costs) / len(costs)
    spread = 0.0 if low == 0 else (high - low) / low * 100.0
    ordered = sorted(costs)
    half = len(ordered) // 2
    median = ordered[half] if len(ordered) % 2 else (ordered[half - 1] + ordered[half]) / 2
    basis = 'billed' if all(row['billed'] for row in rows) else 'cold @1h'
    print('   cost, {}: total {}   median {}   mean {}   band {} to {}   spread {:.0f}%'
          .format(basis, money(sum(costs)), money(median), money(mean), money(low),
                  money(high), spread))
    if len(costs) < 2:
        print('   one run is a draw, not a measurement: nothing here is a band')
    else:
        print('   a difference smaller than the spread is not a finding')


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('ledger', help='the file holding one run directory per line')
    parser.add_argument('--after', type=int, metavar='N',
                        help='report the Nth run and the running total')
    parser.add_argument('--total', type=int, default=0, help='how many runs were asked for')
    parser.add_argument('--band', action='store_true', help='report the spread over all of them')
    args = parser.parse_args()

    rows = rows_of(args.ledger)
    if args.band:
        band(rows, args.total or len(rows))
    elif args.after:
        after(rows, args.after, args.total or args.after)
    else:
        parser.error('one of --after or --band is required')
    return 0


if __name__ == '__main__':
    sys.exit(main())
