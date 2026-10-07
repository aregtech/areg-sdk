#!/usr/bin/env python3
# ===========================================================================
# Writes the <Layout> block of a State Machine document from its state tree.
#
#   python3 tools/agent/fsml_layout.py src/services/Gate.fsml
#   python3 tools/agent/fsml_layout.py Gate.fsml --out Gate.laid.fsml
#
# A document written by hand carries no geometry, so an editor has nowhere to
# put the states. This places every state of a level in one column, sizes each
# composite around its children and joins the transitions of a level with a
# two-point edge. The result is not a designed diagram; it is a readable
# starting point that no two states overlap in.
#
# An existing <Layout> is replaced. Everything else in the document is left as
# it was, including comments and attribute order.
#
# Exit code 0 when the document was written, 1 otherwise.
# ===========================================================================
import argparse
import os
import sys
import xml.etree.ElementTree as ET

GRID = 16               # every coordinate is a multiple of this
NODE_WIDTH = 224        # width of a plain state
NODE_HEIGHT = 96        # height of a plain state
START_WIDTH = 64        # width of a Kind="Start" pseudo-state
START_HEIGHT = 32       # height of a Kind="Start" pseudo-state
GAP_Y = 64              # vertical space between two states of one level
PAD = 48                # space a composite keeps around its children


def snap(value):
    """Rounds a coordinate onto the grid."""
    return int(round(value / float(GRID)) * GRID)


def children_of(state):
    """The states one level below, in document order."""
    holder = state.find('StateList')
    return list(holder.findall('State')) if holder is not None else []


def measure(state):
    """Returns the size of one state, growing a composite around its children.

    A composite is laid out in its own coordinate space, so its size only has to
    hold the column of its children plus the padding.
    """
    kids = children_of(state)
    if state.get('Kind') == 'Start':
        return START_WIDTH, START_HEIGHT
    if not kids:
        return NODE_WIDTH, NODE_HEIGHT

    width = NODE_WIDTH
    height = 0
    for index, kid in enumerate(kids):
        kid_width, kid_height = measure(kid)
        width = max(width, kid_width)
        height += kid_height + (GAP_Y if index else 0)
    return width + 2 * PAD, height + 2 * PAD


def place(states, nodes, origin_x=0, origin_y=0):
    """Stacks one level in a single column and recurses into every composite.

    Each level starts at its own origin, because a composite is opened as its own
    view. Sizes come from measure(), so a parent never overlaps a sibling.
    """
    y = origin_y
    for state in states:
        width, height = measure(state)
        nodes.append((state.get('ID'), snap(origin_x), snap(y), snap(width), snap(height)))
        kids = children_of(state)
        if kids:
            place(kids, nodes, PAD, PAD)
        y += height + GAP_Y


def level_edges(states, nodes_by_id):
    """Two-point edges for the transitions whose target sits in the same level.

    A transition that leaves the level has no meaningful pair of points here, so
    it is left without an edge and the editor draws it directly.
    """
    edges = []
    here = {state.get('ID') for state in states}
    for state in states:
        holder = state.find('TransitionList')
        for transition in (holder.findall('Transition') if holder is not None else []):
            target = transition.get('To')
            if target is None or target not in here or state.get('ID') not in nodes_by_id:
                continue
            if target not in nodes_by_id:
                continue
            _, sx, sy, sw, sh = nodes_by_id[state.get('ID')]
            _, tx, ty, tw, th = nodes_by_id[target]
            edges.append((transition.get('ID'),
                          (snap(sx + sw / 2), sy + sh),
                          (snap(tx + tw / 2), ty)))
    return edges


def walk_levels(states, sink):
    """Calls sink() for every level of the tree, the root level first."""
    sink(states)
    for state in states:
        kids = children_of(state)
        if kids:
            walk_levels(kids, sink)


def build_layout(root):
    """Builds the <Layout> element for a parsed document."""
    top = root.find('StateList')
    states = list(top.findall('State')) if top is not None else []
    if not states:
        return None

    nodes = []
    place(states, nodes)
    nodes_by_id = {node[0]: node for node in nodes}

    edges = []
    walk_levels(states, lambda level: edges.extend(level_edges(level, nodes_by_id)))

    layout = ET.Element('Layout', {'GridSize': str(GRID), 'GridVisible': 'true'})

    # One view for the document, and one for every composite state.
    views = ET.SubElement(layout, 'ViewList')
    overview = root.find('Overview')
    owners = [overview.get('ID')] if overview is not None else []

    def collect(level):
        owners.extend(state.get('ID') for state in level if children_of(state))

    walk_levels(states, collect)
    for owner in owners:
        if owner:
            ET.SubElement(views, 'View', {'Owner': owner, 'Zoom': '100',
                                          'X': '0', 'Y': '0'})

    node_list = ET.SubElement(layout, 'NodeList')
    for owner, x, y, width, height in nodes:
        ET.SubElement(node_list, 'Node', {'Owner': owner, 'X': str(x), 'Y': str(y),
                                          'Width': str(width), 'Height': str(height)})

    edge_list = ET.SubElement(layout, 'EdgeList')
    for owner, first, second in edges:
        edge = ET.SubElement(edge_list, 'Edge', {'Owner': owner})
        for point in (first, second):
            ET.SubElement(edge, 'Point', {'X': str(point[0]), 'Y': str(point[1])})

    return layout


def render(layout, indent):
    """Serializes the layout with the indentation the documents use."""
    lines = []
    step = ' ' * 4

    def emit(element, depth):
        pad = indent + step * depth
        attributes = ''.join(' {}="{}"'.format(k, v) for k, v in element.attrib.items())
        kids = list(element)
        if not kids:
            lines.append('{}<{}{}/>'.format(pad, element.tag, attributes))
            return
        lines.append('{}<{}{}>'.format(pad, element.tag, attributes))
        for kid in kids:
            emit(kid, depth + 1)
        lines.append('{}</{}>'.format(pad, element.tag))

    emit(layout, 0)
    return '\n'.join(lines)


def main():
    parser = argparse.ArgumentParser(
        description='Write the <Layout> block of a .fsml document from its states.')
    parser.add_argument('document', help='the .fsml document')
    parser.add_argument('--out', help='write here instead of in place')
    args = parser.parse_args()

    if not os.path.isfile(args.document):
        print('no such document: ' + args.document, file=sys.stderr)
        return 1

    with open(args.document, encoding='utf-8') as handle:
        text = handle.read()

    try:
        root = ET.fromstring(text)
    except ET.ParseError as error:
        print('not valid XML: {}'.format(error), file=sys.stderr)
        return 1

    layout = build_layout(root)
    if layout is None:
        print('the document declares no state', file=sys.stderr)
        return 1

    block = render(layout, ' ' * 4)
    closing = '</StateMachine>'
    start = text.find('<Layout')
    if start != -1:
        end = text.find('</Layout>')
        end = text.find('\n', end) + 1 if end != -1 else text.find('/>', start) + 3
        line_start = text.rfind('\n', 0, start) + 1
        text = text[:line_start] + block + '\n' + text[end:]
    else:
        cut = text.rfind(closing)
        if cut == -1:
            print('no </StateMachine> in the document', file=sys.stderr)
            return 1
        text = text[:cut] + block + '\n' + text[cut:]

    target = args.out or args.document
    with open(target, 'w', encoding='utf-8', newline='') as handle:
        handle.write(text)

    nodes = len(layout.find('NodeList'))
    edges = len(layout.find('EdgeList'))
    print('{}: {} nodes, {} edges'.format(target, nodes, edges))
    return 0


if __name__ == '__main__':
    sys.exit(main())
