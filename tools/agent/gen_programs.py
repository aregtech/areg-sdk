"""Write an application of several programs from the "programs" block of design.json.

Each program is an executable in its own folder under src/, with its own main() and
model. A component of a program provides a service (as one or more named instances),
drives the scenario as a consumer of one, or watches one; any component also uses other
services through a client member per provider role. Every rule is a TODO(you) marker,
and worksheet.txt carries every one of them, as for an application of one service.

    python3 gen_skeleton.py --programs --spec design.json [--force]

A module of gen_skeleton.py, which runs it for --programs.
"""
import json
import os
import re
import sys

import gen_docs
import gen_skeleton as gs


def fail(message):
    # Output already printed is flushed first, so the error follows what it is about.
    sys.stdout.flush()
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(1)


def program_dir(program):
    return program['name'] + '/'


class Component:
    """One component of one program, with everything its class is written from."""

    def __init__(self, program, kind, iface, entry):
        self.program = program
        self.kind = kind
        self.iface = iface
        self.entry = entry
        self.roles = []
        self.role = None
        self.uses = []
        self.machine = None
        self.machine_doc = None
        self.cls = None
        self.steps = ()
        self.hold = None
        self.name = entry.get('name')
        self.thread = entry.get('thread')


def load_project(specs):
    loaded = [(path, gen_docs.load_spec(path)[0]) for path in specs]
    return gen_docs.merge(loaded)


def components_of(project, specs, services_dir):
    """Every component, its class named, its documents read."""
    ifaces = {}

    def interface(name):
        if name not in ifaces:
            path = os.path.join(services_dir, name + '.siml')
            if not os.path.isfile(path):
                fail('{} does not exist: the documents are written before the '
                     'programs, by build_project.py --spec'.format(path))
            ifaces[name] = (gs.Interface(path), path)
        return ifaces[name]

    # A machine runs in the provider of the service that names it. With none named,
    # the one machine no other machine hosts runs in the one provided service, as an
    # application of one service has it.
    pairs = [(service['name'], machine['name'])
             for service, machine in gen_docs.paired(project)]
    if not pairs:
        hosted = set(inner.get('name') if isinstance(inner, dict) else inner
                     for entry in project['machines'] if isinstance(entry, dict)
                     for inner in entry.get('submachines') or [])
        top = [entry['name'] for entry in project['machines']
               if isinstance(entry, dict) and entry.get('name') not in hosted]
        provided = [service for _, _, kind, service in gen_docs.program_components(project)
                    if kind == 'provides']
        if len(top) == 1 and len(provided) == 1:
            pairs = [(provided[0], top[0])]
    machines = {}
    for service, name in pairs:
        path = os.path.join(services_dir, name + '.fsml')
        found = gs.Interface(path)
        found.load_imports()
        machines[service] = (found, path)

    found = []
    for program, entry, kind, service in gen_docs.program_components(project):
        iface, _ = interface(service)
        component = Component(program, kind, iface, entry)
        if kind == 'provides':
            component.roles = list(entry.get('roles') or [gs.provider_name(iface)])
            if service in machines:
                component.machine, component.machine_doc = machines[service]
        else:
            component.role = entry.get('role') or gen_docs.consumed_role(project, service)
        # Entries naming one service, each with its own role, are one client class.
        merged = {}
        for used, roles in gen_docs.component_uses(entry, project):
            merged.setdefault(used, [])
            merged[used] += [role for role in roles if role not in merged[used]]
        for used, roles in merged.items():
            component.uses.append((interface(used)[0], roles))
        found.append(component)

    # A class name is a file name, and a section of the worksheet is told apart by
    # its file, so every class of the project gets a name of its own.
    watched = {}
    used = {}
    for component in found:
        if component.kind == 'watches':
            watched[component.iface.name] = watched.get(component.iface.name, 0) + 1
        for iface, _ in component.uses:
            used[iface.name] = used.get(iface.name, 0) + 1
    for component in found:
        prefix = gs.pascal(component.program['name'])
        if component.kind == 'provides':
            component.cls = gs.provider_name(component.iface)
        elif component.kind == 'drives':
            component.cls = gs.consumer_name(component.iface)
            component.steps = gs.steps_of(specs, component.iface)
            component.hold = gs.peer_hold(component.steps)
        else:
            component.cls = (prefix if watched[component.iface.name] > 1 else '') + \
                component.iface.name + 'Watcher'
        component.clients = []
        for iface, roles in component.uses:
            name = (prefix if used[iface.name] > 1 else '') + iface.name + 'Client'
            component.clients.append((iface, roles, name))
    names = {}
    for component in found:
        for name in [component.cls] + [c[2] for c in component.clients]:
            if name in names:
                fail('two components of the design would both be the class {}: {} and '
                     'program "{}". Give the second its own program, or let one '
                     'component use the service for both'
                     .format(name, names[name], component.program['name']))
            names[name] = 'program "{}"'.format(component.program['name'])
    return found, ifaces


def member_of(role):
    """The member a client of one provider role is held in."""
    return 'm' + gs.pascal(role)


def client_class(owner, iface, roles, cls, reconnect, driving=False):
    """The client of one used service: a consumer base, subscribed, every answer a marker.

    A provider that goes away is waited for reconnect seconds, the "driver" setting of
    that service, and the section that runs then decides; 0 waits for ever.
    """
    pad = ' ' * (len(cls) + 13)
    lines = ['class {} final : public    {}ConsumerBase'.format(cls, iface.name),
             '{}, private   areg::TimerConsumer'.format(pad),
             '{',
             'public:',
             '    {}(const areg::String & role, areg::ComponentThread & thread, {} & owner)'
             .format(cls, owner),
             '        : {}ConsumerBase(role, thread)'.format(iface.name),
             '        , areg::TimerConsumer()',
             '        , mOwner(owner)',
             '        , mRole(role)',
             '        , mThread(thread)',
             '        , mDeadline(static_cast<areg::TimerConsumer &>(*this), role)',
             '    { }',
             '']
    early = driving and bool(iface.attributes)
    if early:
        lines += ['    //! Runs each update body once on the value held, when an update arrived',
                  '    //! before the steps of the owner began.',
                  '    void deliver_early()',
                  '    {',
                  '        if (mEarly == false)',
                  '        {',
                  '            return;',
                  '        }',
                  '',
                  '        mEarly = false;']
        for attr_name, _ in iface.attributes:
            lines += ['        {',
                      '            areg::DataState lateState{ areg::DataState::DataIsInvalid };',
                      '            const auto lateValue = {}(lateState);'.format(
                          iface.spell('attribute', attr_name, 'get')),
                      '            {}(lateValue, lateState);'.format(
                          iface.spell('attribute', attr_name, 'on_update')),
                      '        }']
        lines += ['    }',
                  '']
    lines += ['protected:',
             '    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final',
             '    {',
             '        bool result{ false };',
             '        if ({}ConsumerBase::service_connected(status, proxy))'.format(iface.name),
             '        {',
             '            result = true;',
             '            if (areg::is_service_connected(status))',
             '            {']
    for attr_name, _ in iface.attributes:
        lines.append('                {}(true);'.format(
            iface.spell('attribute', attr_name, 'notify')))
    for name, _ in iface.broadcasts:
        lines.append('                {}(true);'.format(iface.spell('broadcast', name, 'notify')))
    lines += ['                mDeadline.stop_timer();']
    if driving:
        lines.append('                mOwner.client_connected();')
    lines += [gs.marker('connected', 'what {} does once this provider is connected; '
                        'mRole names it'.format(owner), 16),
              '            }',
              '            else if ((status == areg::ServiceConnectionState::Disconnected) ||',
              '                     (status == areg::ServiceConnectionState::ConnectionLost))',
              '            {',
              '                // The framework reconnects; the deadline is how long it is '
              'waited for.',
              '                if (is_quitting() == false)',
              '                {',
              gs.marker('lost', 'what losing this provider means to {} while it is '
                        'waited for; never quit here'.format(owner), 20),
              '                }',
              '',
              '                if ((cReconnectSeconds != 0) && (is_quitting() == false))',
              '                {',
              '                    mDeadline.stop_timer();',
              '                    mDeadline.start_timer(cReconnectSeconds * 1000, '
              'static_cast<areg::DispatcherThread &>(mThread),',
              '                                          areg::TimerBase::ONE_TIME);',
              '                }',
              '            }',
              '        }',
              '',
              '        return result;',
              '    }',
              '',
              '    void process_timer(areg::Timer & timer) final',
              '    {',
              '        if (&timer == &mDeadline)',
              '        {',
              gs.marker('gone', 'what it means to {} that this provider did not come '
                        'back within cReconnectSeconds'.format(owner), 12),
              gs.placeholder('            std::cerr << "FAIL: " << mRole.as_string() << '
                             '" did not come back" << std::endl;'),
              gs.placeholder('            quit_with(1);'),
              '        }',
              '    }',
              '']
    for name, _ in iface.responses:
        lines += ['    void {}({}) final'.format(iface.spell('response', name),
                                                iface.generated_params('response', name)),
                  '    {',
                  gs.marker(iface.spell('response', name),
                            'what this answer means to {}'.format(owner)),
                  '    }',
                  '']
    for name, _ in iface.requests:
        lines += ['    void {}({}) final'.format(
                      iface.spell('request', name, 'failed'),
                      iface.generated_params('request', name, 'failed').strip()),
                  '    {',
                  '        std::cerr << mRole.as_string() << ": request {} failed, reason " '
                  '<< areg::as_string(reason) << std::endl;'.format(name),
                  '    }',
                  '']
    for name, _ in iface.broadcasts:
        lines += ['    void {}({}) final'.format(iface.spell('broadcast', name),
                                                iface.generated_params('broadcast', name)),
                  '    {',
                  gs.marker(iface.spell('broadcast', name),
                            'what this broadcast means to {}'.format(owner)),
                  '    }',
                  '']
    for attr_name, _ in iface.attributes:
        lines += ['    void {}({}) final'.format(
                      iface.spell('attribute', attr_name, 'on_update'),
                      iface.generated_params('attribute', attr_name, 'on_update').strip()),
                  '    {',
                  '        if (state == areg::DataState::DataIsOK)',
                  '        {']
        if early:
            lines += ['            if (mOwner.mStep == {}::Step::Start)'.format(owner),
                      '            {',
                      '                mEarly = true;',
                      '                return;',
                      '            }',
                      '']
        lines += [gs.marker('update_' + iface.spell('attribute', attr_name, 'get'),
                            'the new value is ready to use' + (
                                '; one that arrives before the steps begin runs as they '
                                'begin' if early else ''), 12),
                  '        }',
                  '    }',
                  '']
    lines += ['private:',
              '    {} & mOwner;    //!< The component this client works for.'.format(owner),
              '    const areg::String mRole;    //!< The provider role this client uses.',
              '    areg::ComponentThread & mThread;    //!< The thread its timer fires on.',
              '    areg::Timer mDeadline;    //!< How long a lost provider is waited for.',
              '    //! Seconds a lost provider is waited for. 0 waits for ever.',
              '    static constexpr uint32_t cReconnectSeconds{{ {} }};'.format(reconnect),
              '']
    if early:
        lines += ['    bool mEarly{ false };    //!< True once an update waits for the steps to begin.',
                  '']
    lines += [
              '    {}() = delete;'.format(cls),
              '    AREG_NOCOPY_NOMOVE({});'.format(cls),
              '};']
    return lines


def with_clients(lines, component):
    """The class lines of a component, holding one client member per used role."""
    if not component.clients:
        return lines
    lines = list(lines)
    # Every client may read the owner's private members and helpers.
    opening = lines.index('{')
    lines[opening + 1:opening + 1] = ['    friend class {};'.format(name)
                                      for _, _, name in component.clients]
    # The members are initialised after every other initialiser, in declaration order.
    head = next(i for i, line in enumerate(lines)
                if line.startswith('    {}(const areg::ComponentEntry'.format(component.cls)))
    last = head
    while gs.INIT_LINE.match(lines[last + 1]):
        last += 1
    inits = []
    members = []
    for iface, roles, name in component.clients:
        for role in roles:
            inits.append('        , {}("{}", owner, self())'.format(member_of(role), role))
            members.append('    {} {};    //!< The {} provider "{}".'
                           .format(name, member_of(role), iface.name, role))
    lines[last + 1:last + 1] = inits
    deleted = lines.index('    {}() = delete;'.format(component.cls))
    lines[deleted:deleted] = members + ['']
    if component.kind == 'drives' and component.steps:
        # The steps begin once this service and every used provider are connected,
        # whichever of them connects last.
        start = lines.index('                if (mStep == Step::Start)')
        lines[start] = '                if ((mStep == Step::Start) && clients_connected())'
        held = [member_of(role) for _, roles, _ in component.clients for role in roles]
        early = [member_of(role) for iface, roles, _ in component.clients
                 if iface.attributes for role in roles]
        lines[start + 3:start + 3] = ['                    {}.deliver_early();'.format(member)
                                      for member in early]
        deleted = lines.index('    {}() = delete;'.format(component.cls))
        lines[deleted:deleted] = [
            '    //! True once every used provider is connected.',
            '    bool clients_connected() const',
            '    {',
            '        return {};'.format(' && '.join('{}.is_connected()'.format(m)
                                                    for m in held)),
            '    }',
            '',
            '    //! Begins the scenario when the last used provider connects after this one.',
            '    void client_connected()',
            '    {',
            '        if (mConnected && (mStep == Step::Start) && clients_connected())',
            '        {',
            '            begin(Step::{});'.format(component.steps[0]['enum'])] + [
            '            {}.deliver_early();'.format(member) for member in early] + [
            '        }',
            '    }',
            '']
    return lines


def watcher_class(iface, cls, driver):
    """A consumer that subscribes and reacts, and ends only when a rule of its own says so."""
    lines = ['class {} final : public    areg::Component'.format(cls),
             '{}, protected {}ConsumerBase'.format(' ' * (len(cls) + 13), iface.name),
             '{}, private   areg::TimerConsumer'.format(' ' * (len(cls) + 13)),
             '{',
             'public:',
             '    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(cls),
             '        : areg::Component(entry, owner)',
             '        , {}ConsumerBase(entry.mDependencyServices[0].mRoleName, owner)'.format(iface.name),
             '        , areg::TimerConsumer()',
             '        , mDeadline(static_cast<areg::TimerConsumer &>(self()), "Deadline")',
             '    { }',
             '',
             'protected:',
             '    void startup_component(areg::ComponentThread & thread) final',
             '    {',
             '        areg::Component::startup_component(thread);',
             '        arm_deadline(cConnectSeconds);',
             '    }',
             '',
             '    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final',
             '    {',
             '        bool result{ false };',
             '        if ({}ConsumerBase::service_connected(status, proxy))'.format(iface.name),
             '        {',
             '            result = true;',
             '            if (areg::is_service_connected(status))',
             '            {',
             '                mDeadline.stop_timer();',
             '                mConnected = true;']
    for attr_name, _ in iface.attributes:
        lines.append('                {}(true);'.format(
            iface.spell('attribute', attr_name, 'notify')))
    for name, _ in iface.broadcasts:
        lines.append('                {}(true);'.format(iface.spell('broadcast', name, 'notify')))
    lines += ['            }',
              '            else if ((status == areg::ServiceConnectionState::Disconnected) ||',
              '                     (status == areg::ServiceConnectionState::ConnectionLost))',
              '            {',
              '                // The provider went away. The framework reconnects and',
              '                // calls this again; the reconnect deadline is the exit.',
              '                if (is_quitting() == false)',
              '                {',
              gs.marker('watch_lost', 'what losing the provider means to this watcher', 20),
              '                    arm_deadline(cReconnectSeconds);',
              '                }',
              '            }',
              '            else if ((status == areg::ServiceConnectionState::Rejected) ||',
              '                     (status == areg::ServiceConnectionState::Shutdown))',
              '            {',
              '                mDeadline.stop_timer();',
              '                std::cerr << "service is " << areg::as_string(status)',
              '                          << ", giving up" << std::endl;',
              '                quit_with(1);',
              '            }',
              '        }',
              '',
              '        return result;',
              '    }',
              '',
              '    void process_timer(areg::Timer & timer) final',
              '    {',
              '        if (&timer == &mDeadline)',
              '        {',
              '            std::cerr << "FAIL: " << (mConnected ? "the provider did not come '
              'back within the reconnect deadline"',
              '                                                : "no provider connected '
              'within the connect deadline") << std::endl;',
              '            quit_with(1);',
              '        }',
              '    }',
              '']
    for name, _ in iface.responses:
        lines += ['    void {}({}) final'.format(iface.spell('response', name),
                                                iface.generated_params('response', name)),
                  '    {',
                  gs.marker('watch_' + iface.spell('response', name),
                            'what this answer means to the watcher'),
                  '    }',
                  '']
    for name, _ in iface.requests:
        lines += ['    void {}({}) final'.format(
                      iface.spell('request', name, 'failed'),
                      iface.generated_params('request', name, 'failed').strip()),
                  '    {',
                  '        std::cerr << "request {} failed, reason " '
                  '<< areg::as_string(reason) << std::endl;'.format(name),
                  '        quit_with(1);',
                  '    }',
                  '']
    for name, _ in iface.broadcasts:
        lines += ['    void {}({}) final'.format(iface.spell('broadcast', name),
                                                iface.generated_params('broadcast', name)),
                  '    {',
                  gs.marker('watch_' + iface.spell('broadcast', name),
                            'what this broadcast means to the watcher; quit_with(0) ends '
                            'it'),
                  '    }',
                  '']
    for attr_name, _ in iface.attributes:
        lines += ['    void {}({}) final'.format(
                      iface.spell('attribute', attr_name, 'on_update'),
                      iface.generated_params('attribute', attr_name, 'on_update').strip()),
                  '    {',
                  '        if (state == areg::DataState::DataIsOK)',
                  '        {',
                  gs.marker('watch_update_' + iface.spell('attribute', attr_name, 'get'),
                            'what the new value means to the watcher; quit_with(0) ends '
                            'it', 12),
                  '        }',
                  '    }',
                  '']
    lines += ['private:',
              '    //! This component as a reference, for a member initialiser that takes one.',
              '    inline {} & self()'.format(cls),
              '    {   return (*this); }',
              '',
              '    //! Starts the deadline timer for this many seconds. 0 stops it and',
              '    //! waits for ever.',
              '    void arm_deadline(uint32_t seconds)',
              '    {',
              '        mDeadline.stop_timer();',
              '        if (seconds != 0)',
              '        {',
              '            mDeadline.start_timer(seconds * 1000,',
              '                                  static_cast<areg::DispatcherThread &>'
              '(master_thread()),',
              '                                  areg::TimerBase::ONE_TIME);',
              '        }',
              '    }',
              '',
              '    areg::Timer  mDeadline;   //!< Ends the run when no provider is there.',
              '    bool         mConnected{ false };   //!< True once the service has '
              'connected.',
              '',
              '    //! Seconds to wait for the provider to appear. 0 waits for ever.',
              '    static constexpr uint32_t cConnectSeconds{{ {} }};'
              .format(driver['connect_seconds']),
              '    //! Seconds to wait for it to come back. 0 waits for ever.',
              '    static constexpr uint32_t cReconnectSeconds{{ {} }};'
              .format(driver['reconnect_seconds']),
              '',
              '    {}() = delete;'.format(cls),
              '    AREG_NOCOPY_NOMOVE({});'.format(cls),
              '};']
    return lines


def component_classes(component, specs, include_root):
    """The files of one component and of the clients it holds, under its program folder."""
    iface = component.iface
    folder = program_dir(component.program)
    machine = component.machine
    if component.kind == 'provides':
        lines = gs.provider_class(iface, component.cls, machine, gs.timers_of(specs, iface))
        if len(component.roles) > 1:
            lines = [line.replace('an attribute is invalid until it is set once',
                                  'an attribute is invalid until it is set once; '
                                  'role_name() says which instance this is')
                     for line in lines]
        bases = ['#include "{}/{}ProviderBase.hpp"'.format(include_root, iface.name)]
        if machine:
            bases += ['#include "{}/{}ActionHandler.hpp"'.format(include_root, machine.name),
                      '#include "{}/{}FSM.hpp"'.format(include_root, machine.name)]
            bases += ['#include "{}/{}ActionHandler.hpp"'.format(
                os.path.dirname(document).replace('\\', '/'), inner.name)
                for inner, document, _ in machine.imports]
        slot, hint = 'provider_state', None
        prelude = gs.QUIT_DECLARATION
        brief = 'Provider of the {} service.'.format(iface.name)
    elif component.kind == 'drives':
        lines = gs.consumer_class(iface, component.cls, component.steps,
                                  gs.driver_of(specs, iface), component.hold)
        bases = ['#include "{}/{}ConsumerBase.hpp"'.format(include_root, iface.name)]
        slot = 'consumer_state'
        hint = ('the members and helpers your checks need, defined here, or one // line '
                'saying none is needed; the step the scenario is on is mStep already'
                if component.steps else None)
        prelude = gs.QUIT_DECLARATION + (gs.HOLD_DECLARATION if component.hold else [])
        brief = 'Consumer of the {} service; it drives the scenario.'.format(iface.name)
    else:
        lines = watcher_class(iface, component.cls, gs.driver_of(specs, iface))
        bases = ['#include "{}/{}ConsumerBase.hpp"'.format(include_root, iface.name)]
        slot, hint = 'watcher_state', None
        prelude = gs.QUIT_DECLARATION
        brief = 'Watcher of the {} service: it subscribes and reacts.'.format(iface.name)
    lines = with_clients(lines, component)
    includes = gs.class_includes(iface, machine) + gs.timer_includes(lines) + [''] + bases
    includes += ['#include "{}.hpp"'.format(name) for _, _, name in component.clients]
    files = gs.component_files(component.cls, brief, includes, lines, slot,
                               prelude, **({'state_hint': hint} if hint else {}))
    produced = [(folder + name, text) for name, text in files]
    for used, roles, name in component.clients:
        client = client_class(component.cls, used, roles, name,
                              gs.driver_of(specs, used)['reconnect_seconds'],
                              component.kind == 'drives' and bool(component.steps))
        client_includes = gs.class_includes(used) + gs.timer_includes(client) + [
            '', '#include "{}/{}ConsumerBase.hpp"'.format(include_root, used.name)]
        prelude_lines = gs.QUIT_DECLARATION + ['class {};'.format(component.cls), '']
        files = gs.component_files(name, 'What {} uses of the {} service: one member per '
                                   'provider role.'.format(component.cls, used.name),
                                   client_includes, client, name_slot(name),
                                   prelude_lines,
                                   state_hint='the members a client of {} keeps, or one // '
                                              'line saying none is needed'.format(used.name))
        produced += [(folder + file_name,
                      text.replace('#include "{}.hpp"\n'.format(name),
                                   '#include "{}.hpp"\n#include "{}.hpp"\n'
                                   .format(name, component.cls), 1)
                      if file_name.endswith('.cpp') else text)
                     for file_name, text in files]
    return produced


def name_slot(cls):
    return gs.snake(cls) + '_state'


def hosted(program, components, placed):
    """What one program registers in one placement, as (component, role or None, thread)."""
    found = []
    for c in components:
        if c.kind == 'provides':
            for role in c.roles:
                where, thread = placed.get(role, (c.program['name'], c.thread))
                if where == program['name']:
                    found.append((c, role, thread or c.thread))
        else:
            where, thread = placed.get(c.name, (c.program['name'], c.thread)) if c.name \
                else (c.program['name'], c.thread)
            if where == program['name']:
                found.append((c, None, thread or c.thread))
    return found


def model_lines(model, instances, local=None):
    """One model: its threads, each with the components registered in it.

    With local, the name of a function that builds it at run time instead, for a model
    the command line picks: one source holds one static model.
    """
    threads = {}
    for c, role, thread in instances:
        if c.kind == 'provides':
            name = thread or '{}Thread'.format(role)
            lines = ['        BEGIN_REGISTER_COMPONENT("{}", {})'.format(role, c.cls),
                     '            REGISTER_IMPLEMENT_SERVICE({}::ServiceName, '
                     '{}::InterfaceVersion)'.format(c.iface.name, c.iface.name),
                     '        END_REGISTER_COMPONENT("{}")'.format(role)]
        else:
            name = thread or '{}Thread'.format(c.cls)
            member = '_' + gs.snake(c.cls)
            lines = ['        BEGIN_REGISTER_COMPONENT({}, {})'.format(member, c.cls),
                     '            REGISTER_DEPENDENCY("{}")'.format(c.role),
                     '        END_REGISTER_COMPONENT({})'.format(member)]
        threads.setdefault(name, []).extend(lines)
    body = []
    for name, lines in threads.items():
        body += ['    BEGIN_REGISTER_THREAD("{}")'.format(name)] + lines + \
                ['    END_REGISTER_THREAD("{}")'.format(name)]
    if local is None:
        return ['BEGIN_MODEL({})'.format(model)] + body + ['END_MODEL({})'.format(model), '']
    return (['//! Builds the model of one deployment, before it is loaded.',
             'void {}()'.format(local),
             '{',
             '    BEGIN_MODEL_LOCAL({})'.format(model)] +
            ['    ' + line for line in body] +
            ['    END_MODEL_LOCAL({})'.format(model), '}', ''])


def program_main(program, components, placements):
    """The main.cpp of one program: its models, one per deployment, and main()."""
    name = program['name']
    per = [(deployment, hosted(program, components, placed))
           for deployment, placed in placements]
    mine = []
    for _, instances in per:
        for c, _, _ in instances:
            if c not in mine:
                mine.append(c)
    driver = next((c for c in mine if c.kind == 'drives'), None)
    chosen = len(placements) > 1
    lines = ['/**',
             ' * \\file    {}main.cpp'.format(program_dir(program)),
             ' * \\brief   The {} program. The model and main(); the components are in '
             'their own files.'.format(name),
             ' **/'] + gs.MAIN_INCLUDES + ['']
    if driver is None:
        lines += gs.CONSOLE_INCLUDES + (['#include <cstring>'] if chosen else []) + ['']
    elif driver.hold or chosen:
        lines += (['#include <iostream>'] if chosen else []) + ['#include <cstring>', '']
    lines += ['#include "{}{}.hpp"'.format('' if c.program is program else
                                           '../' + program_dir(c.program), c.cls)
              for c in mine] + ['']
    lines += gs.EXIT_CODE
    if driver is not None and driver.hold:
        lines += gs.HOLD_CODE
    lines += ['constexpr char const _modelName[]{{ "{}Model" }};'.format(gs.pascal(name))]
    for deployment, _ in per[1:]:
        lines.append('constexpr char const _model{}[]{{ "{}Model{}" }};'
                     .format(gs.pascal(deployment), gs.pascal(name), gs.pascal(deployment)))
    lines.append('')
    consumers = [c for c in mine if c.kind != 'provides']
    if consumers:
        lines.append('// A unique role name lets several consumer processes run at the same time.')
        for c in consumers:
            lines.append('const areg::String _{}(areg::generate_name("{}"));'
                         .format(gs.snake(c.cls), c.cls))
        lines.append('')
    # One model is registered: a second that names the same threads and roles is
    # refused, so with deployments each is built only when it is the one chosen.
    lines += model_lines('_modelName', per[0][1], 'build_model_default' if chosen else None)
    for deployment, instances in per[1:]:
        lines += model_lines('_model' + gs.pascal(deployment), instances,
                             'build_model_' + gs.snake(deployment).lower())
    if chosen:
        lines += ['//! The model of the deployment named on the command line, the first',
                  '//! when none is named, or nullptr for a name no deployment has.',
                  'const char * deployment_model(int argc, char * argv[])',
                  '{',
                  '    const char * name{{ "{}" }};'.format(per[0][0]),
                  '    for (int i = 1; i + 1 < argc; ++i)',
                  '    {',
                  '        if (std::strcmp(argv[i], "--deployment") == 0)',
                  '        {',
                  '            name = argv[i + 1];',
                  '        }',
                  '    }',
                  '',
                  '    const char * model{ nullptr };',
                  '    if (std::strcmp(name, "{}") == 0)'.format(per[0][0]),
                  '    {',
                  '        build_model_default();',
                  '        model = _modelName;',
                  '    }']
        for deployment, _ in per[1:]:
            lines += ['    else if (std::strcmp(name, "{}") == 0)'.format(deployment),
                      '    {',
                      '        {}();'.format('build_model_' + gs.snake(deployment).lower()),
                      '        model = _model{};'.format(gs.pascal(deployment)),
                      '    }']
        lines += ['',
                  '    return model;',
                  '}',
                  '']
    body = gs.MAIN_BODY if driver is None else gs.HOLD_MAIN if driver.hold else gs.EXIT_MAIN
    if chosen:
        selected = ['int main(int argc, char * argv[])',
                    '{',
                    '    const char * model{ deployment_model(argc, argv) };',
                    '    if (model == nullptr)',
                    '    {',
                    '        std::cerr << "unknown --deployment; the deployments: {}" << std::endl;'
                    .format(', '.join(d for d, _ in per)),
                    '        return 2;',
                    '    }',
                    '']
        rest = body[2:] if body[0].startswith('int main') else body
        lines += selected + [line.replace('(_modelName)', '(model)') for line in rest]
    else:
        lines += body
    return (program_dir(program) + 'main.cpp', '\n'.join(lines)), mine


EXECUTABLE = gs.EXECUTABLE
DOCUMENT = gs.DOCUMENT


def write_cmake(path, programs, produced, hosts=None):
    """One executable line per program, naming its main() and every source it compiles.

    A source the caller added to a program's line is kept. The scaffold's two
    executables are replaced; a line of the caller's own is never touched.
    """
    if not os.path.isfile(path):
        return []
    with open(path, encoding='utf-8') as handle:
        lines = handle.read().splitlines()
    library, project = None, None
    names = set(p['name'] for p in programs)
    for line in lines:
        found = DOCUMENT.match(line)
        if found and library is None:
            library = found.group(2)
        found = EXECUTABLE.match(line)
        if found:
            library = library or found.group(3)
            exe = found.group(2)
            for suffix in ['provider', 'consumer'] + sorted(names, key=len, reverse=True):
                if exe.endswith('_' + suffix) and project is None:
                    project = exe[:-len(suffix) - 1]
    executables = [found.group(2) for found in map(EXECUTABLE.match, lines) if found]
    if project is None and len(executables) == 1:
        project = executables[0]
    if library is None or project is None:
        fail('{} names no executable of the scaffold, so the project name is unknown'
             .format(path))
    wanted = {}
    for program in programs:
        folder = program_dir(program)
        sources = [folder + 'main.cpp'] + [n for n, _ in produced
                                           if n.startswith(folder) and n.endswith('.cpp')
                                           and not n.endswith('main.cpp')]
        # A class another program owns, which a deployment places here, is compiled
        # here too, from where it is.
        for c in (hosts or {}).get(program['name'], []):
            if c.program is not program:
                home = program_dir(c.program)
                sources += [home + c.cls + '.cpp'] + \
                    [home + name + '.cpp' for _, _, name in c.clients]
        wanted['{}_{}'.format(project, program['name'])] = sources
    ours = set(wanted) | {project + '_provider', project + '_consumer', project}
    kept, changed, extra = [], [], {}
    for line in lines:
        found = EXECUTABLE.match(line)
        if found and found.group(2) in ours:
            listed = found.group(4).split()
            extra[found.group(2)] = [n for n in listed
                                     if n not in wanted.get(found.group(2), [])
                                     and n not in gs.SCAFFOLD_MAINS
                                     and n not in gs.SCAFFOLD_MAINS.values()
                                     and os.path.isfile(os.path.join(os.path.dirname(path), n))]
            continue
        kept.append(line)
    for exe, sources in wanted.items():
        line = 'macro_declare_executable({} {} {})'.format(
            exe, library, ' '.join(sources + extra.get(exe, [])))
        kept.append(line)
    if kept != lines:
        with open(path, 'w', encoding='utf-8') as handle:
            handle.write('\n'.join(kept) + '\n')
        changed = ['{}: {} executable(s), one per program'.format(
            os.path.basename(path), len(wanted))]
    return changed, project


def drop_placeholders(out, produced):
    """Remove the scaffold's provider.cpp and consumer.cpp, which no line compiles now."""
    classes = set(os.path.basename(n)[:-4] for n, _ in produced if n.endswith('.hpp'))
    for old in list(gs.SCAFFOLD_MAINS) + ['main.cpp']:
        path = os.path.join(out, old)
        if not os.path.isfile(path):
            continue
        with open(path, encoding='utf-8', errors='ignore') as handle:
            text = handle.read()
        if 'BEGIN_MODEL' not in text or not gs.SCAFFOLD_BASE.search(text) \
                or any(name in text for name in classes):
            continue
        os.remove(path)
        print('removed {} -- the placeholder the programs replaced'
              .format(path.replace('\\', '/')))


def write_scenarios(path, programs, components, project_name, specs, deployments=()):
    """The normal run of every program, the console quit of each server and a peer loss."""
    if not os.path.exists(path):
        return
    try:
        with open(path, encoding='utf-8') as handle:
            document = json.load(handle)
        first = document['scenarios'][0]
    except (ValueError, OSError, KeyError, IndexError, TypeError):
        print('  kept   {} -- it is not the file setup_project.py wrote'.format(path))
        return
    if not first.pop('scaffold', False):
        print('  kept   {} -- its expectations are yours, not the scaffold\'s'.format(path))
        return
    driver = next((c for c in components if c.kind == 'drives'), None)
    lead = driver.program if driver else programs[-1]
    order = [p for p in programs if p is not lead] + [lead]

    def binary(program):
        return '{}_{}'.format(project_name, program['name'])

    def kinds(program):
        return set(c.kind for c in components if c.program is program)

    procs = []
    for program in order:
        spec = {'binary': binary(program), 'name': program['name']}
        watcher = 'watches' in kinds(program)
        if program is lead or watcher or not (driver and driver.steps):
            spec['expect'] = [gs.SCENARIO_TODO.format(
                gs.expect_slot('smoke', {'binary': binary(program), 'name': program['name']}))]
        else:
            spec['expect'] = []
        if program is lead:
            spec['lead'] = True
            spec['exit'] = 0
        procs.append(spec)
    requests = len(driver.iface.requests) if driver else 0
    scenarios = [{'name': 'smoke', 'timeout': 60 + requests * gs.STEP_INTERVAL_MS // 1000,
                  'router': True, 'procs': procs}]
    servers = [p for p in programs if 'provides' in kinds(p) and p is not lead]
    for index, program in enumerate(servers):
        scenarios.append({'name': gs.QUIT_SCENARIO if index == 0
                          else '{}-{}'.format(gs.QUIT_SCENARIO, program['name']),
                          'timeout': 30, 'router': True,
                          'procs': [{'binary': binary(program), 'name': program['name'],
                                     'lead': True, 'stdin': ['-q'], 'exit': 0}]})
    # Every other deployment runs the same programs, started with its name; the
    # driver's own checks are the proof, so it has no expectation of its own.
    for deployment in deployments:
        scenarios.append({'name': 'smoke-{}'.format(deployment),
                          'timeout': scenarios[0]['timeout'], 'router': True,
                          'procs': [dict({'binary': p['binary'], 'name': p['name'],
                                          'args': ['--deployment', deployment]},
                                         **({'lead': True, 'exit': 0} if p.get('lead') else {}))
                                    for p in procs]})
    reconnect = gs.driver_of(specs, driver.iface)['reconnect_seconds'] if driver else 0
    if driver and reconnect:
        lost = next((c.program for c in components if c.kind == 'provides'
                     and driver.role in c.roles), None)
        held = driver.hold or gs.held_step(list(driver.steps))
        trigger = '^step {}$'.format(re.escape(held['name'])) if held else \
            gs.STOP_TODO.format(gs.stop_slot(gs.PEER_LOST_SCENARIO))
        lost_procs = []
        for program in order:
            spec = {'binary': binary(program), 'name': program['name']}
            if program is lead:
                if driver.hold:
                    spec['args'] = [gs.HOLD_FLAG]
                spec.update({'lead': True, 'exit': 1})
            lost_procs.append(spec)
        if lost is not None and lost is not lead:
            scenarios.append({'name': gs.PEER_LOST_SCENARIO,
                              'timeout': scenarios[0]['timeout'] + reconnect,
                              'router': True,
                              'stop': {'proc': lost['name'], 'after': trigger,
                                       'signal': 'kill'},
                              'procs': lost_procs})
    document['scenarios'] = scenarios + [s for s in document['scenarios'][1:]
                                         if s.get('name') not in
                                         set(x['name'] for x in scenarios)
                                         and s.get('name') not in ('quit', 'peer-lost')]
    with open(path, 'w', encoding='utf-8') as handle:
        json.dump(document, handle, indent=2)
        handle.write('\n')
    print('wrote  {}'.format(path))
    print('  {} program(s), router on: "smoke" runs them all and {} leads it; each'
          .format(len(order), lead['name']))
    print('  "expect" hole is a section of the worksheet. {} quit scenario(s) and {}'
          .format(len(servers), 'a peer-lost scenario' if driver and reconnect
                  else 'no peer-lost scenario'))
    print('  were added, and need nothing from you.')


def wiring(programs, components, deployments=()):
    """The worksheet lines saying which program holds what, and how its parts meet."""
    lines = ['The programs, and how their components reach each other:']
    for program in programs:
        parts = []
        for c in components:
            if c.program is not program:
                continue
            said = c.cls
            if c.kind == 'provides':
                said += ' as ' + ', '.join(c.roles)
            elif c.kind == 'drives':
                said += ' drives the scenario on ' + c.role
            else:
                said += ' watches ' + c.role
            for iface, roles, name in c.clients:
                said += '; {} {}'.format(name, ', '.join(member_of(r) for r in roles))
            if c.thread:
                said += ' (thread {})'.format(c.thread)
            parts.append(said)
        lines.append('  {}: {}'.format(program['name'], '. '.join(parts)))
    for deployment in deployments:
        moves = ', '.join('{} in {}'.format(k, v)
                          for k, v in sorted((deployment.get('place') or {}).items()))
        lines.append('  deployment {}: {}; main() takes --deployment {}, and a role '
                     'is reached the same way wherever it runs'
                     .format(deployment['name'], moves or 'as above', deployment['name']))
    if any(c.clients for c in components):
        lines += ['A component calls a used provider through its member, as',
                  '{}.request_<name>(...), and reads its attributes there. A client\'s'
                  .format(member_of(next(r for c in components for _, rs, _ in c.clients
                                         for r in rs))),
                  'sections reach their component as mOwner, private members included;',
                  'quit_with() stays a free call. mRole names the provider the client talks',
                  'to. A driver begins its steps once every provider it uses is connected.']
    if any(c.clients for c in components if c.kind == 'provides'):
        lines += ['A request answered only after a used provider answers keeps its caller:',
                  '  const areg::SessionID session{ unblock_current_request() };',
                  'and once the answer is known, the same component sends it to that caller:',
                  '  if (prepare_response(session)) { response_<name>(...); }',
                  'Several such requests may be open at once, each with its own session.']
    return lines


def main(argv=None):
    import argparse
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--spec', action='append', default=[], required=True)
    parser.add_argument('--services', default=os.path.join('src', 'services'))
    parser.add_argument('--out', default='src')
    parser.add_argument('--scenarios', default='scenarios.json')
    parser.add_argument('--force', action='store_true')
    args = parser.parse_args(argv)

    project = load_project(args.spec)
    programs = project.get('programs') or []
    if not programs:
        fail('the design has no "programs" block')
    gen_docs.check_programs(project)
    components, ifaces = components_of(project, args.spec, args.services)
    include_root = os.path.relpath(os.path.abspath(args.services),
                                   os.getcwd()).replace('\\', '/')
    produced = []
    for component in components:
        produced += component_classes(component, args.spec, include_root)
    placements = [('default', gen_docs.placement(project))] + \
        [(d['name'], gen_docs.placement(project, d)) for d in project.get('deployments') or []]
    hosts = {}
    for program in programs:
        main_file, mine = program_main(program, components, placements)
        produced.append(main_file)
        hosts[program['name']] = mine

    retained = [(name, gs.write(os.path.join(args.out, name), text, args.force))
                for name, text in produced]
    drop_placeholders(args.out, produced)
    changed, project_name = write_cmake(os.path.join(args.out, 'CMakeLists.txt'),
                                        programs, produced, hosts)
    for change in changed:
        print('  {}/{}'.format(args.out.replace('\\', '/'), change))
    write_scenarios(args.scenarios, programs, components, project_name, args.spec,
                    [name for name, _ in placements[1:]])

    driver = next((c for c in components if c.kind == 'drives'), None)
    contracts = []
    for name, (iface, path) in sorted(ifaces.items()):
        contracts.append((iface, path))
    for component in components:
        if component.machine is not None:
            contracts.append((component.machine, component.machine_doc))
    first = not os.path.isfile(gs.WORKSHEET)
    written = gs.write_worksheet(retained, args.out, contracts[0][0], contracts[0][1],
                                 None, None, args.scenarios,
                                 driver.steps if driver else (),
                                 contracts=contracts,
                                 extra=wiring(programs, components,
                                              project.get('deployments') or []))
    gs.print_todos(retained, args.out, written is not None,
                   len(gs.scenario_holes(args.scenarios)), args.scenarios, first,
                   written or ())
    if first:
        print(gs.APP_NOTE)
    return 0

