#ifndef AREG_COMPONENT_NEREGISTRY_HPP
#define AREG_COMPONENT_NEREGISTRY_HPP
/************************************************************************
 * \file        areg/component/NERegistry.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, collection of Service Registry
 *              classes to initialize and load application objects.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/Version.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/TELinkedList.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NEMemory.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
namespace NERegistry
{
    class ServiceEntry;
    class ServiceList;
    class ComponentThreadEntry;
    class ComponentThreadList;
    class WorkerThreadEntry;
    class WorkerThreadList;
    class DependencyEntry;
    class DependencyList;
    class ComponentEntry;
    class ComponentList;
    class Model;
}

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. ServiceEntry;
 *              2. ServiceList;
 *              3. ComponentThreadEntry;
 *              4. ComponentThreadList;
 *              5. WorkerThreadEntry;
 *              6. WorkerThreadList;
 *              7. DependencyEntry;
 *              8. DependencyList;
 *              9. ComponentEntry;
 *             10. ComponentList;
 *             11. Model;
 *          These classes are declared as part of Registry and used when
 *          Model is defined and created, where object have descriptions
 *          of Service Interfaces, Components, Threads and dependencies.
 *          For more information, see descriptions bellow.
 ************************************************************************/

/************************************************************************
 * Dependencies
 ************************************************************************/
class Component;
class WorkerThread;
class ComponentThread;
class IEWorkerThreadConsumer;

/************************************************************************
 * Global types
 ************************************************************************/

/**
 * \brief   Type of Component Load Function
 *          Used and called to instantiate and load component
 **/
typedef Component*    (*FuncCreateComponent)  ( const NERegistry::ComponentEntry & /*entry*/, ComponentThread & /*componentThread*/);

/**
 * \brief   Type of Component Unload Function
 *          Used and called to unload and delete component
 **/
typedef void            (*FuncDeleteComponent)  (Component & /*componentItem*/, const NERegistry::ComponentEntry & /*entry*/);

//////////////////////////////////////////////////////////////////////////
// NERegistry namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       This namespace contains collection of classes to register
 *              components, service interfaces, threads and point
 *              dependencies. These is used when defining Model object
 *              and used to start initialization process when Model
 *              object is loading.
 * 
 **/
namespace NERegistry
{

//////////////////////////////////////////////////////////////////////////
// NERegistry::ServiceEntry class declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::ServiceEnty, defines Server side of implemented 
     *          Service Interfaces inComponent. It contains name of 
     *          Service Interface and implemented Version. One component 
     *          can contain more than one Service Entry, indicating all
     *          implemented Service Interfaces. The list of implemented
     *          Services are listed in the Service List.
     **/
    class AREG_API ServiceEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor. Should not be directly used.
         *          Creates invalid Service Entry and required by Array List object.
         *          Invalid Service Entry has name NERegistry::INVALID_SERVICE_ENTRY_NAME
         **/
        ServiceEntry( void );

        /**
         * \brief   Initialize service entry by given name and version numbers.
         *          The passed serviceName and the version number should be valid.
         *          The valid service name should not be empty or equal to NERegistry::INVALID_SERVICE_ENTRY_NAME.
         *          The version is valid, when either major or minor version numbers are not zero.
         * \param   serviceName     The name of implemented service interface. 
         * \param   major           The major version number of implemented interface. 
         * \param   minor           The minor version number of implemented interface
         * \param   patch           The patch version number of implemented interface
         **/
        ServiceEntry( const char* serviceName, unsigned int major, unsigned int minor, unsigned int patch );

        /**
         * \brief   Initialize service interface entry by given name and version object.
         *          The passed serviceName and the Version should be valid.
         *          The valid Service name should not be empty or equal to NERegistry::INVALID_SERVICE_ENTRY_NAME.
         *          The Version is valid if major or minor version is not zero.
         * \param   serviceName The name of service interface
         * \param   version     The version object of implemented service interface,
         *                      containing major, minor and patch version numbers
         **/
        ServiceEntry( const char* serviceName, const Version & version );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        ServiceEntry( const ServiceEntry & src );

        /**
         * \brief   Destructor.
         **/
        ~ServiceEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Service Entry data from given source.
         * \param   src     The source of data to copy
         **/
        const NERegistry::ServiceEntry & operator = ( const NERegistry::ServiceEntry & src );

        /**
         * \brief   Checks the equality of two Service Entries and returns true if they are equal.
         *          It compares Service names and Versions.
         * \param   other   The Service Entry object to compare.
         **/
        bool operator == ( const NERegistry::ServiceEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceEntry class, Attributes
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if service entry is valid.
         *          The Service Entry is valid if name is not empty and version is valid.
         **/
        bool isValid( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The name of implemented Service Interface
         **/
        String     mName;

        /**
         * \brief   The Version of implemented Service.
         **/
        Version   mVersion;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::ServiceList, defines list of Service Entries. 
     *          It is a list of all implemented Service Interfaces in the Component.
     **/
    class AREG_API ServiceList : public TEArrayList<NERegistry::ServiceEntry, const NERegistry::ServiceEntry &>
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        ServiceList( void );

        /**
         * \brief   Creates Service List and adds specified Service Entry to the list as first element.
         * \param   entry   The Service Entry to set as first element in the list.
         **/
        ServiceList( const NERegistry::ServiceEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        ServiceList( const NERegistry::ServiceList & src);

        /**
         * \brief   Destructor
         **/
        virtual ~ServiceList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceList class, operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Service Entries from given source of Service List.
         * \param   src     The source of Service List, containing Service Entries.
         **/
        const NERegistry::ServiceList & operator = ( const NERegistry::ServiceList & src );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if service entry list is valid
         **/
        bool isValid( void ) const;

        /**
         * \brief   Searches Service Entry having given name. Returns valid
         *          Service Entry if found, otherwise returns invalid entry
         *          NERegistry::INVALID_SERVICE_ENTRY.
         *          There should not be several Service Entries with same
         *          Service name. If a Service Interface should be implemented
         *          more than one time, they should be registered in 
         *          different Components having different Role Names and
         *          running in different threads.
         * \param   serviceName The name of Service Entry to search.
         * \return  Returns valid Service Entry if found. Otherwise returns
         *          invalid entry NERegistry::INVALID_SERVICE_ENTRY.
         **/
        const NERegistry::ServiceEntry & getService( const char* serviceName ) const;

        /**
         * \brief   Searches Service Entry by given Service Name name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   serviceName The Service Name of Service Entry to search.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findService( const char* serviceName ) const;

        /**
         * \brief   Searches specified Service Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Service Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findService(const NERegistry::ServiceEntry & entry) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::WorkerThreadEntry, definines Worker Thread.
     *          It should be part of binding Component. The Worker Threads
     *          are created when Component is crated.
     **/
    class AREG_API WorkerThreadEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates invalid Worker Thread Entry.
         **/
        WorkerThreadEntry( void );

        /**
         * \brief   Initialize Worker Thread Entry by given name and specifying the name of Master Thread.
         *          The Master Thread is the thread where Component is registered and created.
         *          Since one component may have different role names and
         *          Component may have more than one worker thread, it should be
         *          possible to identify consumer object by consumer name and thread name
         *          to configure worker thread properly.
         * \param   masterThreadName    The name of Master Thread where Component is created and running.
         * \param   workerThreadName    The name of Worker Thread of Component. The name should be unique.
         * \param   compRoleName        The name of Component (Role Name) where consumer is registered.
         * \param   compConsumerName    The name of Consumer object to configure, it should not be same as Component name.
         **/
        WorkerThreadEntry( const char * masterThreadName, const char* workerThreadName, const char * compRoleName, const char* compConsumerName );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        WorkerThreadEntry( const NERegistry::WorkerThreadEntry & src );

        /**
         * \brief   Destructor
         **/
        ~WorkerThreadEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Worker Thread Entry data from given source. 
         * \param   src     The source of data to copy
         **/
        const NERegistry::WorkerThreadEntry & operator = ( const NERegistry::WorkerThreadEntry & src );

        /**
         * \brief   Checks equality of two Worker Thread Entries and returns true if they are equal.
         *          It compares Worker Thread name and Consumer Name.
         * \param   other   The Worker Thread Entry to compare.
         **/
        bool operator == ( const NERegistry::WorkerThreadEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class, Attributes
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Worker Thread Entry is valid.
         *          The Entry is valid if neither Worker Thread nor Consumer names are empty.
         **/
        bool isValid( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class, Member variables.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The name of Worker Thread
         **/
        String mThreadName;
        /**
         * \brief   The name of Worker Thread Consumer.
         **/
        String mConsumerName;
   };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::WorkerThreadList. Defines list of Worker Thread Entries.
     *          It is a list of all Worker Threads binded with one Component.
     **/
    class AREG_API WorkerThreadList    : public TEArrayList<NERegistry::WorkerThreadEntry, const NERegistry::WorkerThreadEntry&>
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        WorkerThreadList( void );

        /**
         * \brief   Creates Worker Thread List and adds specified Worker Thread Entry to the list as first element.
         * \param   entry   The Worker Thread Entry to set as first element in the list.
         **/
        WorkerThreadList( const NERegistry::WorkerThreadEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        WorkerThreadList( const NERegistry::WorkerThreadList & src );

        /**
         * \brief   Destructor
         **/
        virtual ~WorkerThreadList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadList class, Operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Worker Thread Entries from given source of Worker Thread List.
         * \param   src     The source of Worker Thread List, containing Worker Thread Entries.
         **/
        const NERegistry::WorkerThreadList & operator = ( const NERegistry::WorkerThreadList & src );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadList class, Attributes and operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if list is valid
         **/
        bool isValid( void ) const;

        /**
         * \brief   Searches Worker Thread Entry having given name. Returns valid
         *          Worker Thread Entry if found, otherwise returns invalid entry
         *          NERegistry::INVALID_WORKER_THREAD_ENTRY.
         *          There should not be several Worker Thread Entries with same name. 
         * \param   threadName  The name of Worker Thread Entry to search.
         * \return  Returns valid Worker Thread Entry if found. Otherwise returns
         *          invalid entry NERegistry::INVALID_WORKER_THREAD_ENTRY.
         **/
        const NERegistry::WorkerThreadEntry & getWorkerThread( const char* threadName ) const;

        /**
         * \brief   Searches Worker Thread Entry by given thread name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   threadName  The Worker Thread name to search entry
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findThread( const char* threadName ) const;

        /**
         * \brief   Searches specified Worker Thread Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Worker Thread Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findThread( const NERegistry::WorkerThreadEntry & entry ) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::DependencyEntry. Defines dependency of client Component
     *          from Server Component. It is a part of Client Component, but
     *          contains Role Name of dependent Server Component.
     *
     *          For example, if a Component ABC contains Dependency Entry
     *          with Role Name XYZ, means that the Component ABC is a 
     *          Client of Component XYZThe Service Interface server
     *          implementation is on Component XYZ side, and the Client of
     *          the same Service Interface is on Component ABC side
     **/
    class AREG_API DependencyEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Creates empty Dependency Entry and required by Array List object
         **/
        DependencyEntry( void );

        /**
         * \brief   Sets the role name of dependent Component
         * \param   roleName    The Role Name of dependent Component to set.
         **/
        DependencyEntry( const char* roleName );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        DependencyEntry( const NERegistry::DependencyEntry & src );

        /**
         * \brief   Destructor
         **/
        ~DependencyEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class, Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Copies  Dependency Entry  data from given source. 
         * \param   src     The source of data to copy
         **/
        const NERegistry::DependencyEntry & operator = ( const DependencyEntry & src );

        /**
         * \brief   Checks equality of two Dependency Entries and returns true if they are equal.
         *          It compares dependency Role names.
         * \param   other   The Dependency Entry object to compare.
         **/
        bool operator == ( const NERegistry::DependencyEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class, Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns true if entry is valid. The entry is valid if role name is not empty
         **/
        bool isValid( void ) const;

        /**
         * \brief   Sets the role name of dependent service.
         * \param   roleName    The role name of dependent service. If empty, invalidates dependency.
         **/
        void setDependentService( const char * roleName );

        /**
         * \brief   Returns the role name of dependent service.
         **/
        const String & getDepdendentService( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The Role Name of dependent Component
         **/
        String     mRoleName;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::DependencyList, Defines list of Dependency Entries. 
     *          It is a list of all dependent Components. Can be empty, 
     *          if Component does not have dependency.
     *          The dependency is needed to specify client-server relationship
     *          between Components. Every Dependency Entry is specifying
     *          client of specified Component.
     **/
    class AREG_API DependencyList   : public TEArrayList<NERegistry::DependencyEntry, const NERegistry::DependencyEntry&>
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Creates empty list of dependent services.
         **/
        DependencyList( void );

        /**
         * \brief   If passed entry is valid, creates a dependency list with one entry.
         * \param   entry   The entry of dependent service. If entry is empty (invalid), 
         *                  the parameter is ignored and empty list is created.
         **/
        DependencyList( const NERegistry::DependencyEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        DependencyList( const NERegistry::DependencyList & src );

        /**
         * \brief   Destructor
         **/
        virtual ~DependencyList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyList class, Operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Dependency Entries from given source of Dependency List.
         * \param   src     The source of Dependency List, containing Dependency Entries.
         **/
        const NERegistry::DependencyList & operator = ( const NERegistry::DependencyList & src );

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Searches Dependency Entry having given role name. Returns valid
         *          Worker Thread Entry if found, otherwise returns empty Dependency Entry.
         *          There should not be several Dependency Entries with same name. 
         * \param   roleName    The name of dependent Component Entry to search.
         * \return  Returns valid Dependency Entry if found. Otherwise returns
         *          empty entry.
         **/
        const NERegistry::DependencyEntry & getDependency( const char* roleName ) const;

        /**
         * \brief   Searches Dependency Entry by given role name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   roleName    The Role Name of dependent Component
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findDependency( const char* roleName ) const;

        /**
         * \brief   Searches specified Dependency Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Dependency Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findDependency( const NERegistry::DependencyEntry & entry ) const;

        /**
         * \brief   Returns true if dependency list is valid.
         **/
        bool isValid( void ) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::ComponentEntry, defines Component Entry.
     *          The Component Entry may contain list of implemented Services,
     *          list of Worker Threads and list of Dependencies, if they exist.
     *          The mandatory for component is the Role Name. All others are listed
     *          up to the Component design.
     **/
    class AREG_API ComponentEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates invalid Component Entry.
         **/
        ComponentEntry( void );

        /**
         * \brief   Initialize Component Entry by given Role Name, component thread, component create and delete methods,
         *          list of implemented service interfaces, list of dependent services and list of worker threads.
         * \param   masterThreadName    The name of Master Thread where the component is created and run
         * \param   roleName            The Role Name of Component
         * \param   funcCreate          Pointer of component create function
         * \param   funcDelete          Pointer of component delete function
         **/
        ComponentEntry( const char * masterThreadName, const char* roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete );

        /**
         * \brief   Initialize Component Entry by given Role Name, component thread, component create and delete methods,
         *          list of implemented service interfaces, list of dependent services and list of worker threads.
         *          The list parameters can be empty.
         * \param   masterThreadName    The name of Master Thread where the component is created and run
         * \param   roleName            The Role Name of Component
         * \param   funcCreate          Pointer of component create function
         * \param   funcDelete          Pointer of component delete function
         * \param   serviceList         The list of implemented service interfaces
         * \param   dependencyList      The list of dependent service interfaces
         * \param   workerList          The list of worker threads
         **/
        ComponentEntry(   const char * masterThreadName
                        , const char * roleName
                        , FuncCreateComponent funcCreate
                        , FuncDeleteComponent funcDelete
                        , const NERegistry::ServiceList & serviceList
                        , const NERegistry::DependencyList & dependencyList
                        , const NERegistry::WorkerThreadList & workerList);

        /**
         * \brief   Initialize Component Entry by given Role Name, component thread, component create and delete methods,
         *          single entry of implemented service interface, single entry of dependent service and single entry of worker threads.
         *          The entries can be empty.
         * \param   masterThreadName    The name of Master Thread where the component is created and run
         * \param   roleName            The Role Name of Component
         * \param   funcCreate          Pointer of component create function
         * \param   funcDelete          Pointer of component delete function
         * \param   serviceList         The entry of implemented service interface, if there is any
         * \param   dependencyList      The entry of dependent service interface, if there is any
         * \param   workerList          The entry of worker thread, if there is any
         **/
        ComponentEntry(   const char * masterThreadName
                        , const char * roleName
                        , FuncCreateComponent funcCreate
                        , FuncDeleteComponent funcDelete
                        , const NERegistry::ServiceEntry & service
                        , const NERegistry::DependencyEntry & dependency
                        , const NERegistry::WorkerThreadEntry & worker);

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        ComponentEntry( const NERegistry::ComponentEntry & src );

        /**
         * \brief   Destructor
         **/
        ~ComponentEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Component Entry data from given source.
         * \param   src     The source of data to copy
         **/
        const NERegistry::ComponentEntry & operator = ( const NERegistry::ComponentEntry & src );

        /**
         * \brief   Checks equality of two Component Entries and returns true if they are equal.
         *          It compares Master Thread name, Role Name, and pointers of Create and Delete Component functions.
         * \param   other   The Component Entry to compare.
         **/
        bool operator == ( const NERegistry::ComponentEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Component Entry is valid.
         *          The Entry is valid if Role Name of Component Entry is not empty.
         **/
        bool isValid( void ) const;

        /**
         * \brief   Adds supported Service Entry in Component Entry object. 
         *          The Service Entry is defining implemented Service Interface,
         *          supported by Component. A Component may have more than one different 
         *          Service Interfaces implementation.
         * \param   entry   The Service Entry, defining implemented 
         *                  Service Interface in Component.
         **/
        void addSupportedService( const NERegistry::ServiceEntry & entry );
        
        /**
         * \brief   Adds List of supported Service Entries in Component Entry object.
         *          Every Service Entry in the List is defining implemented Service Interface,
         *          supported by Component. A Component may have more than one different 
         *          Service Interfaces implementation.
         * \param   serviceList The list of Service Entries. Every Service Entry is defining
         *                      implemented Service Interfaces in Component.
         **/
        void addSupportedService( const NERegistry::ServiceList & serviceList );

        /**
         * \brief   Adds supported service interface entry in the component entry object.
         *          The serviceName should be unique within component entry context.
         * \param   serviceName The name of supported service interface. 
         *                      If not unique, no new entry added.
         * \param   version     The supported interface version.
         * \return  Returns either new added instance or the existing instance of supported service interface.
         *          The uniqueness is checked within component entry context and ignores the version number.
         **/
        NERegistry::ServiceEntry & addSupportedService( const char * serviceName, const Version & version );

        /**
         * \brief   Searches given supported service. If found, removes from list.
         * \param   serviceName The name of supported service
         * \return  Returns true if found and remove supported service entry in the list.
         **/
        bool removeSupportedService( const char * serviceName );

        /**
         * \brief   Searches Service Entry in the existing list of Component Entry
         *          and returns the zero-based valid position if found. 
         *          Otherwise, returns -1, if cannot find Service Entry in the list.
         * \param   entry   The Service Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int findSupportedService( const NERegistry::ServiceEntry & entry ) const;

        /**
         * \brief   By given name, searches Service Entry in the existing list of 
         *          Component Entry and returns the zero-based valid position if found.
         *          Returns -1, if cannot find Service Entry in the list.
         * \param   serviceName The Name of Service Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int findSupportedService( const char* serviceName ) const;

        /**
         * \brief   Adds Worker Thread Entry in Component Entry object. 
         *          The Worker Thread Entry is defining Worker Thread object,
         *          binded with Component. A Component may have zero or more
         *          binded Worker Threads.
         * \param   entry   The Worker Thread Entry, defining Worker Thread
         *                  object binded with Component.
         **/
        void addWorkerThread( const NERegistry::WorkerThreadEntry & entry );

        /**
         * \brief   Adds List of Worker Thread Entries in Component Entry object. 
         *          Every Worker Thread Entry in the List is defining Worker Thread object,
         *          binded with Component. A Component may have zero or more
         *          binded Worker Threads.
         * \param   workerList  The List of Worker Thread Entries, defining list of 
         *                      Worker Thread objects binded with Component.
         **/
        void addWorkerThread( const NERegistry::WorkerThreadList & workerList );

        /**
         * \brief   Searches Worker Thread Entry in the existing list of Component Entry
         *          and returns the valid position in the list. Returns -1, if cannot
         *          find Worker Thread Entry in the list.
         * \param   entry   The Worker Thread Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element. 
         *          Otherwise, returns -1.
         **/
        int findWorkerThread( const NERegistry::WorkerThreadEntry & entry ) const;

        /**
         * \brief   By given name, searches Worker Thread Entry in the existing list of 
         *          Component Entry and returns the valid position in the list. 
         *          Returns -1, if cannot find Worker Thread Entry in the list.
         * \param   workerName  The Name of Worker Thread Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element. 
         *          Otherwise, returns -1.
         **/
        int findWorkerThread( const char* workerName ) const;

        /**
         * \brief   Searches the worker thread entry in the list.
         *          If found, removes it from the list.
         * \param   workerName  The name of worker thread to remove
         * \return  Returns true if worker thread entry found and could remove.
         **/
        bool removeWorkerThread( const char * workerName );

        /**
         * \brief   Adds Dependency Entry in Component Entry object. 
         *          The Dependency Entry is defining Client part of Server Component,
         *          specified by Role Name. A Component may have more than one same
         *          Service Interfaces dependency, but every dependency should
         *          differ by unique Role Name of Server Component.
         *          For example, if Service Interface "QWERT" is implemented in
         *          Components ABC and XYZ (the Role Names are "ABC" and "XYZ"),
         *          then dependency should be defined by Role Names "ABC" and "XYZ",
         *          indicating dependency of same Service Interface "QWERT", but
         *          different implementation in ABC and XYZ Components.
         * \param   entry   The Dependency Entry, defining Role Name of Server Component.
         **/
        void addDependencyService( const NERegistry::DependencyEntry & entry );

        /**
         * \brief   Adds List of Dependency Entries in Component Entry object. 
         *          Every Dependency Entry in the List is defining Client part of Server Component,
         *          specified by Role Name. A Component may have more than one same
         *          Service Interfaces dependency, but every dependency should
         *          differ by unique Role Name of Server Component.
         *          For example, if Service Interface "QWERT" is implemented in
         *          Components ABC and XYZ (the Role Names are "ABC" and "XYZ"),
         *          then dependency should be defined by Role Names "ABC" and "XYZ",
         *          indicating dependency of same Service Interface "QWERT", but
         *          different implementation in ABC and XYZ Components.
         * \param   entry   The Dependency Entry, defining Role Name of Server Component.
         **/
        void addDependencyService( const NERegistry::DependencyList & dependencyList );

        /**
         * \brief   Adds a dependency entry in the component. The passed parameter should be
         *          the name of local or public service.
         * \param   roleName    The name dependent service.
         * \return  Returns the instance of new added or existing dependency of specified service.
         **/
        NERegistry::DependencyEntry & addDependencyService( const char * roleName );

        /**
         * \brief   Searches Dependency Entry in the existing list of Component Entry
         *          and returns the valid position in the list. Returns -1, if cannot
         *          find Dependency Entry in the list.
         * \param   entry   The Dependency Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int findDependencyService( const NERegistry::DependencyEntry & entry ) const;

        /**
         * \brief   Searches Service in dependency service list by given Role Name.
         *          If found, removes entry from the list.
         * \param   serviceName     The name of dependency service role name to search.
         * \return  Returns true if found entry and could remove it.
         **/
        bool removeDependencyService( const char * roleName );

        /**
         * \brief   By given name, searches Dependency Entry in the existing list of 
         *          Component Entry and returns the valid position in the list. 
         *          Returns -1, if cannot find Dependency Entry in the list.
         * \param   roleName    The Name of Dependency Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int findDependencyService(const char* roleName) const;

        /**
         * \brief   Returns list of supported Services of Component Entry object
         **/
        const NERegistry::ServiceList & getSupportedServices( void ) const;

        /**
         * \brief   Returns list of Worker Threads of Component Entry object
         **/
        const NERegistry::WorkerThreadList & getWorkerThreads( void ) const;

        /**
         * \brief   Returns list of Dependencies of Component Entry object
         **/
        const NERegistry::DependencyList & getDependencyServices( void ) const;

        /**
         * \brief   Sets pointers to create and delete component methods to be called when instantiating component object.
         *          If NULL, it clears existing pointers and no component is instantiated.
         * \param   fnCreate    The pointer to create component method.
         * \param   fnDelete    The pointer to delete component method.
         **/
        void setInstanceMethods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete );

        /**
         * \brief   Sets component data to pass to component create method.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    The system automatically does not free manually allocated space. This means that 
         *          if as a component data a pointer to manually allocated object is passed,
         *          it should be as well manually freed.
         **/
        void setComponentData( NEMemory::uAlign compData );

        /**
         * \brief   Returns the pointer to component data.
         **/
        NEMemory::uAlign getComponentData( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The Role Name of Component
         **/
        String              mRoleName;
        /**
         * \brief   The Master Thread Entry of Component.
         **/
        String              mThreadName;
        /**
         * \brief   Pointer of function to create component
         **/
        FuncCreateComponent mFuncCreate;
        /**
         * \brief   Pointer of function to delete component
         **/
        FuncDeleteComponent mFuncDelete;
        /**
         * \brief   List of supported services
         **/
        ServiceList         mSupportedServices;
        /**
         * \brief   List of worker threads
         **/
        WorkerThreadList    mWorkerThreads;
        /**
         * \brief   List of dependencies
         **/
        DependencyList      mDependencyServices;
        /**
         * \brief   The data to pass to component create method.
         * \note    If data is created by allocating memory (for example, via 'new' operator),
         *          it is not automatically deleted in destructor of Component Registry object.
         **/
        mutable NEMemory::uAlign    mComponentData;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntryList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::ComponentList. Defines list of Component Entries.
     *          It is a list of all Components within one Thread.
     *          All Components should be valid and have unique Role Name.
     *          None of Components should have same implemented Service Interface.
     *          If more than one Components are implementing same Service Interface,
     *          they should be listed in different list of different thread.
     **/
    class AREG_API ComponentList   : public TEArrayList<NERegistry::ComponentEntry, const NERegistry::ComponentEntry&>
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentList class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates empty list.
         **/
        ComponentList( void );

        /**
         * \brief   Creates Component List and adds specified Component Entry to the list as first element.
         * \param   entry   The Component Entry to set as first element in the list.
         **/
        ComponentList(  const NERegistry::ComponentEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        ComponentList( const NERegistry::ComponentList & src );

        /**
         * \brief   Destructor
         **/
        virtual ~ComponentList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentList class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Component Entries from given source of Component List.
         * \param   src     The source of Component List, containing Component Entries.
         **/
        const NERegistry::ComponentList & operator = ( const NERegistry::ComponentList & src );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Component List is valid.
         **/
        bool isValid( void ) const;

        /**
         * \brief   Searches Component Entry having given name. Returns valid
         *          Component Entry if found, otherwise returns invalid entry
         *          NERegistry::INVALID_COMPONENT_ENTRY.
         *          There should not be several Component Entries with same name. 
         * \param   roleName    The name of Component Entry to search.
         * \return  Returns valid Component Entry if found. Otherwise returns
         *          invalid entry NERegistry::INVALID_COMPONENT_ENTRY.
         **/
        const NERegistry::ComponentEntry & getComponent( const char* roleName ) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    The system automatically does not free manually allocated space. This means that 
         *          if as a component data a pointer to manually allocated object is passed,
         *          it should be as well manually freed.
         **/
        bool setComponentData( const char * roleName, NEMemory::uAlign compData );

        /**
         * \brief   Searches Component Entry by given Role Name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   roleName    The Role Name of Component Entry to search.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findComponent( const char* roleName ) const;

        /**
         * \brief   Searches specified Component Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Component Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findComponent(const NERegistry::ComponentEntry& entry) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::ComponentThreadEntry. Defines Master Thread of one or
     *          mode registered Component. All registered Components are 
     *          created in specified thread. One Thread should have at least
     *          one registered Component. All registered Components should have
     *          unique Role Names and different Service Names.
     **/
    class AREG_API ComponentThreadEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates invalid Thread Entry.
         **/
        ComponentThreadEntry( void );

        /**
         * \brief   Initialize Thread Entry with given Thread Name.
         * \param   threadName  The Thread Name to assign.
         **/
        ComponentThreadEntry( const char* threadName );

        /**
         * \brief   Initialize Thread Entry with given Thread Name and given Component List.
         * \param   threadName      The Thread Name to assign.
         * \param   componentList   The List of Component Entries to assign.
         **/
        ComponentThreadEntry( const char* threadName, const NERegistry::ComponentList & componentList );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        ComponentThreadEntry( const NERegistry::ComponentThreadEntry & src );

        /**
         * \brief   Destructor
         **/
        virtual ~ComponentThreadEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Thread Entry data from given source.
         * \param   src     The source of data to copy
         **/
        const NERegistry::ComponentThreadEntry & operator = ( const NERegistry::ComponentThreadEntry & src );

        /**
         * \brief   Checks equality of two Thread Entries and returns true if they are equal.
         *          It compares Thread names.
         * \param   other   The Thread Entry to compare.
         **/
        bool operator == ( const NERegistry::ComponentThreadEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class, Attribute and operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Thread Entry is valid.
         *          The Entry is valid if Name of Thread Entry is not empty.
         **/
        bool isValid( void ) const;

        /**
         * \brief   Adds supported Component Entry in Thread Entry object. 
         *          A Thread may have more than one Component with unique
         *          Role Name, and every Component within one Thread should
         *          have implementation of different Service Interfaces.
         *          If there is a Service Interface with different implementation,
         *          they should be defined in different threads.
         *          For example, if Service Interface "QWERT" is implemented
         *          in Component ABC and XYZ, they should not be in the
         *          same Thread Entry, but can be in different Thread.
         *          Otherwise, same Event will be processed twice in
         *          every Stub of same Service Interface.
         * \param   entry   The Component Entry, defining Component in Thread.
         **/
        void addComponent( const NERegistry::ComponentEntry & entry );

        /**
         * \brief   Adds List of supported Component Entries in Thread Entry object. 
         *          A Thread may have more than one Component with unique
         *          Role Name, and every Component within one Thread should
         *          have implementation of different Service Interfaces.
         *          If there is a Service Interface with different implementation,
         *          they should be defined in different threads.
         *          For example, if Service Interface "QWERT" is implemented
         *          in Component ABC and XYZ, they should not be in the
         *          same Thread Entry, but can be in different Thread.
         *          Otherwise, same Event will be processed twice in
         *          every Stub of same Service Interface.
         * \param   componentList   The List of Component Entries. Every Component Entry is defining
         *                          Component in Thread.
         **/
        void addComponent( const NERegistry::ComponentList & componentList );

        /**
         * \brief   Adds a new component entry with the given role.
         * \param   roleName    The name of component to add. The name must be unique to add new entry.
         * \param   funcCreate  The pointer to the method that instantiates the component.
         * \param   functDelete The pointer to the method that deletes the component.
         * \return  Returns instance of new added or the instance of the existing component entry with
         *          the given role name. The checkup happens only within the current thread list.
         *
         * \note    NOTE:   The method does not check the uniqueness of role name within the entire system or entire
         *                  model. It checks only within the current component thread entry context.
         **/
        NERegistry::ComponentEntry & addComponent( const char * roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete );

        /**
         * \brief   Searches the component entry by given role name.
         *          If found, remove the component entry from the list.
         * \param   roleName    The roleName of service component to search and remove.
         * \return  Returns true if found and could remove the component entry.
         **/
        bool removeComponent( const char * roleName );

        /**
         * \brief   Searches Component Entry in the existing list of Thread Entry
         *          and returns the zero-based valid position if found. 
         *          Otherwise, returns -1, if cannot find Component Entry in the list.
         * \param   entry   The Component Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int findComponentEntry( const NERegistry::ComponentEntry & entry ) const;

        /**
         * \brief   By given name, searches Component Entry in the existing list of 
         *          Thread Entry and returns the zero-based valid position if found.
         *          Returns -1, if cannot find Component Entry in the list.
         * \param   roleName    The Name of Component Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int findComponentEntry( const char* roleName ) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    The system automatically does not free manually allocated space. This means that 
         *          if as a component data a pointer to manually allocated object is passed,
         *          it should be as well manually freed.
         **/
        bool setComponentData( const char * roleName, NEMemory::uAlign compData );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   The name of thread component (thread name)
         **/
        String          mThreadName;

        /**
         * \brief   List of component entries
         **/
        ComponentList   mComponents;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::ComponentThreadList. Defines list of Thread Entries in Model.
     *          All names of Thread should be unique. A Model has more than one
     *          Thread Entry.
     **/
    class AREG_API ComponentThreadList  : public TEArrayList< NERegistry::ComponentThreadEntry, const NERegistry::ComponentThreadEntry & >
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class, Constructors / destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Default constructor.
         **/
        ComponentThreadList( void );

        /**
         * \brief   Creates Component Thread List and adds specified Thread Entry to the list as first element.
         * \param   entry   The Thread Entry to set as first element in the list.
         **/
        ComponentThreadList( const NERegistry::ComponentThreadEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        ComponentThreadList( const NERegistry::ComponentThreadList & src );

        /**
         * \brief   Destructor
         **/
        virtual ~ComponentThreadList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Thread Entries from given source of Thread List.
         * \param   src     The source of Thread List, containing Thread Entries.
         **/
        const NERegistry::ComponentThreadList & operator = ( const NERegistry::ComponentThreadList & src );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Thread List is valid.
         **/
        bool isValid( void ) const;

        /**
         * \brief   Searches Thread Entry having given name. Returns valid
         *          Thread Entry if found, otherwise returns invalid entry
         *          NERegistry::INVALID_THREAD_ENTRY.
         *          There should not be several Thread Entries with same name. 
         * \param   threadName  The name of Thread Entry to search.
         * \return  Returns valid Thread Entry if found. Otherwise returns
         *          invalid entry NERegistry::INVALID_THREAD_ENTRY.
         **/
        const NERegistry::ComponentThreadEntry & getThread( const char* threadName ) const;

        /**
         * \brief   Searches Thread Entry by given Thread Name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   threadName  The Thread Name of Thread Entry to search.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findThread( const char* threadName ) const;

        /**
         * \brief   Searches specified Thread Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Thread Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int findThread( const NERegistry::ComponentThreadEntry & entry ) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::Model. Defines Model of project / binary.
     *          Every project / binary can have only one Model, specifying model of
     *          Threads, Components and dependencies.
     *          If several Models should be defined in application, they should
     *          be specified separately in separate projects / binaries.
     **/
    class AREG_API Model
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates empty Model.
         **/
        Model( void );

        /**
         * \brief   Initialize empty Model and sets given Name.
         **/
        Model( const char* modelName );

        /**
         * \brief   Initialize empty Model and sets given Name.
         **/
        Model( const char* modelName, const ComponentThreadList & threadList );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        Model( const Model & src );

        /**
         * \brief   Destructor
         **/
        ~Model( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Model data from given source, 
         * \param   src     The source of data to copy
         **/
        const NERegistry::Model & operator = ( const NERegistry::Model & src );

        /**
         * \brief   Checks equality of two Models and returns true if they are equal.
         *          It will compare Model names and registered Thread Entries.
         **/
        bool operator == ( const NERegistry::Model & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true, if Mode is valid.
         *          The Model is valid if Name is not empty and it contains at least one thread entry.
         **/
        bool isValid( void ) const;

        /**
         * \brief   Returns the name of model.
         **/
        const String & getModelName( void ) const;

        /**
         * \brief   Searches specified Component Entry  and returns true if 
         *          found specified Component Entry. Otherwise returns false.
         * \param   entry   The Component Entry to search in entire Model.
         * \return  Returns true if found specified Component Entry.
         *          Otherwise returns false.
         **/
        bool hasRegisteredComponent( const NERegistry::ComponentEntry & entry ) const;

        /**
         * \brief   Searches specified Component Entry  and returns true if 
         *          found specified Component Entry by Role Name. Otherwise returns false.
         * \param   roleName    The Role Name of Component Entry to search in entire Model.
         * \return  Returns true if found specified Component Entry.
         *          Otherwise returns false.
         **/
        bool hasRegisteredComponent( const char * roleName ) const;

        /**
         * \brief   Returns true if Model is loaded. Otherwise, returns false.
         **/
        bool isModelLoaded( void ) const;

        /**
         * \brief   Returns the List of Thread Entries.
         **/
        const NERegistry::ComponentThreadList & getThreadList( void ) const;

        /**
         * \brief   Sets flag and marks Model as loaded or unloaded.
         * \param   isLoaded    The flag to set. 
         *                      If true, the Model is marked as loaded.
         *                      If false, the Model is marked as unloaded.
         **/
        void markModelLoaded( bool isLoaded = true );

        /**
         * \brief   Adds supported Thread Entry to the list of Model.
         *          Every Thread Entry should have global unique Name.
         * \param   entry   The Thread Entry to add.
         **/
        void addThread( const NERegistry::ComponentThreadEntry & entry );

        /**
         * \brief   Adds List of supported Thread Entries to the list of Model.
         *          Every Thread Entry in the List should have global unique Name.
         * \param   threadList  The list of Thread Entries to add.
         **/
        void addThread( const NERegistry::ComponentThreadList & threadList );

        /**
         * \brief   Adds new component thread entry in the list if the given thread does not exit.
         * \param   threadName  The name of the thread to add. The name must be unique.
         * \return  Returns the instance of the component thread or returns the instance of the
         *          existing component thread if the thread with the given name is already registered.
         *
         * \note    NOTE:   The method does not check the uniqueness of thread entry in entire system
         *                  and does not check the lists of worker threads. It checks only within current
         *                  model context escaping worker thread names. The caller must take care of uniqueness
         *                  by its self.
         **/
        NERegistry::ComponentThreadEntry & addThread(const char * threadName );

        /**
         * \brief   Searches component thread entry in the list.
         *          If found, removes it.
         * \param   threadName  The name of component thread to search.
         * \return  Returns true if found and could remove the thread entry from the list.
         **/
        bool removeThread( const char * threadName );

        /**
         * \brief   Searches Thread Entry in the existing list of Model
         *          and returns the zero-based valid position if found. 
         *          Otherwise, returns -1, if cannot find Thread Entry in Model.
         * \param   entry   The Thread Entry to search in Model.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int findThread( const NERegistry::ComponentThreadEntry & entry ) const;

        /**
         * \brief   By given name, searches Thread Entry in the existing list of 
         *          Model and returns the zero-based valid position if found.
         *          Returns -1, if cannot find Thread Entry in Model.
         * \param   threadName  The Name of Thread Entry to search in Model.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int findThread(const char* threadName) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    The system automatically does not free manually allocated space. This means that 
         *          if as a component data a pointer to manually allocated object is passed,
         *          it should be as well manually freed.
         **/
        bool setComponentData( const char * roleName, NEMemory::uAlign compData );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   The Name of Model
         **/
        String                  mModelName;

        /**
         * \brief   The List of Thread Entries in Model
         **/
        ComponentThreadList     mModelThreads;

        /**
         * \brief   The Flag, indicating whether model is loaded or not.
         **/
        bool                    mIsLoaded;
    };

//////////////////////////////////////////////////////////////////////////
// NERegistry namespace Predefined Invalid Entries.
//////////////////////////////////////////////////////////////////////////

    /**
      * \brief   NERegistry::INVALID_SERVICE_ENTRY
      *          Predefined invalid Service Entry. 
      **/
     extern AREG_API const NERegistry::ServiceEntry           INVALID_SERVICE_ENTRY;

     /**
      * \brief   NERegistry::INVALID_SERVICE_LIST
      *          Predefined invalid Service List.
      **/
    extern AREG_API const NERegistry::ServiceList             INVALID_SERVICE_LIST;

    /**
     * \brief   NERegistry::INVALID_WORKER_THREAD_ENTRY
     *          Predefined invalid Worker Thread Entry.
     **/
    extern AREG_API const NERegistry::WorkerThreadEntry       INVALID_WORKER_THREAD_ENTRY;

    /**
     * \brief   NERegistry::INVALID_WORKER_THREAD_LIST
     *          Predefined invalid Worker Thread List.
     **/
    extern AREG_API const NERegistry::WorkerThreadList        INVALID_WORKER_THREAD_LIST;

    /**
     * \brief   NERegistry::INVALID_WORKER_THREAD_LIST
     *          Predefined invalid service dependency entry.
     **/
    extern AREG_API const NERegistry::DependencyEntry         INVALID_DEPENDENCY_ENTRY;

    /**
     * \brief   NERegistry::INVALID_WORKER_THREAD_LIST
     *          Predefined invalid service dependency entry.
     **/
    extern AREG_API const NERegistry::DependencyList          INVALID_DEPENDENCY_LIST;

    /**
     * \brief   NERegistry::INVALID_COMPONENT_ENTRY
     *          Predefined Invalid Component Entry.
     **/
    extern AREG_API const NERegistry::ComponentEntry          INVALID_COMPONENT_ENTRY;

    /**
     * \brief   NERegistry::INVALID_COMPONENT_LIST
     *          Predefined invalid Component List.
     **/
    extern AREG_API const NERegistry::ComponentList           INVALID_COMPONENT_LIST;

    /**
     * \brief   NERegistry::INVALID_THREAD_ENTRY
     *          Predefined invalid Thread Entry.
     **/
    extern AREG_API const NERegistry::ComponentThreadEntry    INVALID_THREAD_ENTRY;

    /**
     * \brief   NERegistry::INVALID_THREAD_LIST
     *          Predefined invalid Thread List. Contains single invalid NERegistry::INVALID_THREAD_ENTRY entry.
     **/
    extern AREG_API const NERegistry::ComponentThreadList     INVALID_THREAD_LIST;

    /**
     * \brief   NERegistry::Model::INVALID_MODEL
     *          Predefined invalid Model.
     **/
    extern AREG_API const NERegistry::Model                   INVALID_MODEL;

}

#endif  // AREG_COMPONENT_NEREGISTRY_HPP
