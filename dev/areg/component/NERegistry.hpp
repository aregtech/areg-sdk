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
#include "areg/base/CEVersion.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/TELinkedList.hpp"
#include "areg/base/CEString.hpp"
#include "areg/base/NEMemory.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
namespace NERegistry
{
    class CEServiceEntry;
    class CEServiceList;
    class CEComponentThreadEntry;
    class CEComponentThreadList;
    class CEWorkerThreadEntry;
    class CEWorkerThreadList;
    class CEDependencyEntry;
    class CEDependencyList;
    class CEComponentEntry;
    class CEComponentList;
    class CEModel;
}

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. CEServiceEntry;
 *              2. CEServiceList;
 *              3. CEComponentThreadEntry;
 *              4. CEComponentThreadList;
 *              5. CEWorkerThreadEntry;
 *              6. CEWorkerThreadList;
 *              7. CEDependencyEntry;
 *              8. CEDependencyList;
 *              9. CEComponentEntry;
 *             10. CEComponentList;
 *             11. CEModel;
 *          These classes are declared as part of Registry and used when
 *          Model is defined and created, where object have descriptions
 *          of Service Interfaces, Components, Threads and dependencies.
 *          For more information, see descriptions bellow.
 ************************************************************************/

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEComponent;
class CEWorkerThread;
class CEComponentThread;
class IEWorkerThreadConsumer;

/************************************************************************
 * Global types
 ************************************************************************/

/**
 * \brief   Type of Component Load Function
 *          Used and called to instantiate and load component
 **/
typedef CEComponent*    (*FuncCreateComponent)  ( const NERegistry::CEComponentEntry & /*entry*/, CEComponentThread & /*componentThread*/);

/**
 * \brief   Type of Component Unload Function
 *          Used and called to unload and delete component
 **/
typedef void            (*FuncDeleteComponent)  (CEComponent & /*componentItem*/, const NERegistry::CEComponentEntry & /*entry*/);

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
    // NERegistry::CEServiceEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEServiceEnty, defines Server side of implemented 
     *          Service Interfaces inComponent. It contains name of 
     *          Service Interface and implemented Version. One component 
     *          can contain more than one Service Entry, indicating all
     *          implemented Service Interfaces. The list of implemented
     *          Services are listed in the Service List.
     **/
    class AREG_API CEServiceEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEServiceEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor. Should not be directly used.
         *          Creates invalid Service Entry and required by Array List object.
         *          Invalid Service Entry has name NERegistry::INVALID_SERVICE_ENTRY_NAME
         **/
        CEServiceEntry( void );

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
        CEServiceEntry( const char* serviceName, unsigned int major, unsigned int minor, unsigned int patch );

        /**
         * \brief   Initialize service interface entry by given name and version object.
         *          The passed serviceName and the Version should be valid.
         *          The valid Service name should not be empty or equal to NERegistry::INVALID_SERVICE_ENTRY_NAME.
         *          The Version is valid if major or minor version is not zero.
         * \param   serviceName The name of service interface
         * \param   version     The version object of implemented service interface,
         *                      containing major, minor and patch version numbers
         **/
        CEServiceEntry( const char* serviceName, const CEVersion & version );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEServiceEntry( const CEServiceEntry & src );

        /**
         * \brief   Destructor.
         **/
        ~CEServiceEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEServiceEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Service Entry data from given source.
         * \param   src     The source of data to copy
         **/
        const NERegistry::CEServiceEntry & operator = ( const NERegistry::CEServiceEntry & src );

        /**
         * \brief   Checks the equality of two Service Entries and returns true if they are equal.
         *          It compares Service names and Versions.
         * \param   other   The Service Entry object to compare.
         **/
        bool operator == ( const NERegistry::CEServiceEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEServiceEntry class, Attributes
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if service entry is valid.
         *          The Service Entry is valid if name is not empty and version is valid.
         **/
        bool IsValid( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEServiceEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The name of implemented Service Interface
         **/
        CEString     mName;

        /**
         * \brief   The Version of implemented Service.
         **/
        CEVersion   mVersion;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEServiceList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEServiceList, defines list of Service Entries. 
     *          It is a list of all implemented Service Interfaces in the Component.
     **/
    class AREG_API CEServiceList : public TEArrayList<NERegistry::CEServiceEntry, const NERegistry::CEServiceEntry &>
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEServiceList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        CEServiceList( void );

        /**
         * \brief   Creates Service List and adds specified Service Entry to the list as first element.
         * \param   entry   The Service Entry to set as first element in the list.
         **/
        CEServiceList( const NERegistry::CEServiceEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEServiceList( const NERegistry::CEServiceList & src);

        /**
         * \brief   Destructor
         **/
        virtual ~CEServiceList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEServiceList class, operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Service Entries from given source of Service List.
         * \param   src     The source of Service List, containing Service Entries.
         **/
        const NERegistry::CEServiceList & operator = ( const NERegistry::CEServiceList & src );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEServiceList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if service entry list is valid
         **/
        bool IsValid( void ) const;

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
        const NERegistry::CEServiceEntry & GetService( const char* serviceName ) const;

        /**
         * \brief   Searches Service Entry by given Service Name name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   serviceName The Service Name of Service Entry to search.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchService( const char* serviceName ) const;

        /**
         * \brief   Searches specified Service Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Service Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchService(const NERegistry::CEServiceEntry & entry) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEWorkerThreadEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEWorkerThreadEntry, definines Worker Thread.
     *          It should be part of binding Component. The Worker Threads
     *          are created when Component is crated.
     **/
    class AREG_API CEWorkerThreadEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEWorkerThreadEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates invalid Worker Thread Entry.
         **/
        CEWorkerThreadEntry( void );

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
        CEWorkerThreadEntry( const char * masterThreadName, const char* workerThreadName, const char * compRoleName, const char* compConsumerName );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEWorkerThreadEntry( const NERegistry::CEWorkerThreadEntry & src );

        /**
         * \brief   Destructor
         **/
        ~CEWorkerThreadEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEWorkerThreadEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Worker Thread Entry data from given source. 
         * \param   src     The source of data to copy
         **/
        const NERegistry::CEWorkerThreadEntry & operator = ( const NERegistry::CEWorkerThreadEntry & src );

        /**
         * \brief   Checks equality of two Worker Thread Entries and returns true if they are equal.
         *          It compares Worker Thread name and Consumer Name.
         * \param   other   The Worker Thread Entry to compare.
         **/
        bool operator == ( const NERegistry::CEWorkerThreadEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEWorkerThreadEntry class, Attributes
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Worker Thread Entry is valid.
         *          The Entry is valid if neither Worker Thread nor Consumer names are empty.
         **/
        bool IsValid( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEWorkerThreadEntry class, Member variables.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The name of Worker Thread
         **/
        CEString mThreadName;
        /**
         * \brief   The name of Worker Thread Consumer.
         **/
        CEString mConsumerName;
   };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEWorkerThreadList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEWorkerThreadList. Defines list of Worker Thread Entries.
     *          It is a list of all Worker Threads binded with one Component.
     **/
    class AREG_API CEWorkerThreadList    : public TEArrayList<NERegistry::CEWorkerThreadEntry, const NERegistry::CEWorkerThreadEntry&>
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEWorkerThreadList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        CEWorkerThreadList( void );

        /**
         * \brief   Creates Worker Thread List and adds specified Worker Thread Entry to the list as first element.
         * \param   entry   The Worker Thread Entry to set as first element in the list.
         **/
        CEWorkerThreadList( const NERegistry::CEWorkerThreadEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEWorkerThreadList( const NERegistry::CEWorkerThreadList & src );

        /**
         * \brief   Destructor
         **/
        virtual ~CEWorkerThreadList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEWorkerThreadList class, Operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Worker Thread Entries from given source of Worker Thread List.
         * \param   src     The source of Worker Thread List, containing Worker Thread Entries.
         **/
        const NERegistry::CEWorkerThreadList & operator = ( const NERegistry::CEWorkerThreadList & src );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEWorkerThreadList class, Attributes and operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if list is valid
         **/
        bool IsValid( void ) const;

        /**
         * \brief   Searches Worker Thread Entry having given name. Returns valid
         *          Worker Thread Entry if found, otherwise returns invalid entry
         *          NERegistry::INVALID_WORKER_THREAD_ENTRY.
         *          There should not be several Worker Thread Entries with same name. 
         * \param   threadName  The name of Worker Thread Entry to search.
         * \return  Returns valid Worker Thread Entry if found. Otherwise returns
         *          invalid entry NERegistry::INVALID_WORKER_THREAD_ENTRY.
         **/
        const NERegistry::CEWorkerThreadEntry & GetWorkerThread( const char* threadName ) const;

        /**
         * \brief   Searches Worker Thread Entry by given thread name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   threadName  The Worker Thread name to search entry
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchThread( const char* threadName ) const;

        /**
         * \brief   Searches specified Worker Thread Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Worker Thread Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchThread( const NERegistry::CEWorkerThreadEntry & entry ) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEDependencyEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEDependencyEntry. Defines dependency of client Component
     *          from Server Component. It is a part of Client Component, but
     *          contains Role Name of dependent Server Component.
     *
     *          For example, if a Component ABC contains Dependency Entry
     *          with Role Name XYZ, means that the Component ABC is a 
     *          Client of Component XYZThe Service Interface server
     *          implementation is on Component XYZ side, and the Client of
     *          the same Service Interface is on Component ABC side
     **/
    class AREG_API CEDependencyEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEDependencyEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Creates empty Dependency Entry and required by Array List object
         **/
        CEDependencyEntry( void );

        /**
         * \brief   Sets the role name of dependent Component
         * \param   roleName    The Role Name of dependent Component to set.
         **/
        CEDependencyEntry( const char* roleName );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEDependencyEntry( const NERegistry::CEDependencyEntry & src );

        /**
         * \brief   Destructor
         **/
        ~CEDependencyEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEDependencyEntry class, Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Copies  Dependency Entry  data from given source. 
         * \param   src     The source of data to copy
         **/
        const NERegistry::CEDependencyEntry & operator = ( const CEDependencyEntry & src );

        /**
         * \brief   Checks equality of two Dependency Entries and returns true if they are equal.
         *          It compares dependency Role names.
         * \param   other   The Dependency Entry object to compare.
         **/
        bool operator == ( const NERegistry::CEDependencyEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEDependencyEntry class, Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns true if entry is valid. The entry is valid if role name is not empty
         **/
        bool IsValid( void ) const;

        /**
         * \brief   Sets the role name of dependent service.
         * \param   roleName    The role name of dependent service. If empty, invalidates dependency.
         **/
        void SetDependentService( const char * roleName );

        /**
         * \brief   Returns the role name of dependent service.
         **/
        const CEString & GetDepdendentService( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEDependencyEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The Role Name of dependent Component
         **/
        CEString     mRoleName;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEDependencyList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEDependencyList, Defines list of Dependency Entries. 
     *          It is a list of all dependent Components. Can be empty, 
     *          if Component does not have dependency.
     *          The dependency is needed to specify client-server relationship
     *          between Components. Every Dependency Entry is specifying
     *          client of specified Component.
     **/
    class AREG_API CEDependencyList   : public TEArrayList<NERegistry::CEDependencyEntry, const NERegistry::CEDependencyEntry&>
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEDependencyList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Creates empty list of dependent services.
         **/
        CEDependencyList( void );

        /**
         * \brief   If passed entry is valid, creates a dependency list with one entry.
         * \param   entry   The entry of dependent service. If entry is empty (invalid), 
         *                  the parameter is ignored and empty list is created.
         **/
        CEDependencyList( const NERegistry::CEDependencyEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEDependencyList( const NERegistry::CEDependencyList & src );

        /**
         * \brief   Destructor
         **/
        virtual ~CEDependencyList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEDependencyList class, Operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Dependency Entries from given source of Dependency List.
         * \param   src     The source of Dependency List, containing Dependency Entries.
         **/
        const NERegistry::CEDependencyList & operator = ( const NERegistry::CEDependencyList & src );

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
        const NERegistry::CEDependencyEntry & GetDependency( const char* roleName ) const;

        /**
         * \brief   Searches Dependency Entry by given role name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   roleName    The Role Name of dependent Component
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchDependency( const char* roleName ) const;

        /**
         * \brief   Searches specified Dependency Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Dependency Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchDependency( const NERegistry::CEDependencyEntry & entry ) const;

        /**
         * \brief   Returns true if dependency list is valid.
         **/
        bool IsValid( void ) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEComponentEntry, defines Component Entry.
     *          The Component Entry may contain list of implemented Services,
     *          list of Worker Threads and list of Dependencies, if they exist.
     *          The mandatory for component is the Role Name. All others are listed
     *          up to the Component design.
     **/
    class AREG_API CEComponentEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentEntry class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class CEComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentEntry class, Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates invalid Component Entry.
         **/
        CEComponentEntry( void );

        /**
         * \brief   Initialize Component Entry by given Role Name, component thread, component create and delete methods,
         *          list of implemented service interfaces, list of dependent services and list of worker threads.
         * \param   masterThreadName    The name of Master Thread where the component is created and run
         * \param   roleName            The Role Name of Component
         * \param   funcCreate          Pointer of component create function
         * \param   funcDelete          Pointer of component delete function
         **/
        CEComponentEntry( const char * masterThreadName, const char* roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete );

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
        CEComponentEntry( const char * masterThreadName
                        , const char * roleName
                        , FuncCreateComponent funcCreate
                        , FuncDeleteComponent funcDelete
                        , const NERegistry::CEServiceList & serviceList
                        , const NERegistry::CEDependencyList & dependencyList
                        , const NERegistry::CEWorkerThreadList & workerList);

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
        CEComponentEntry( const char * masterThreadName
                        , const char * roleName
                        , FuncCreateComponent funcCreate
                        , FuncDeleteComponent funcDelete
                        , const NERegistry::CEServiceEntry & service
                        , const NERegistry::CEDependencyEntry & dependency
                        , const NERegistry::CEWorkerThreadEntry & worker);

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEComponentEntry( const NERegistry::CEComponentEntry & src );

        /**
         * \brief   Destructor
         **/
        ~CEComponentEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Component Entry data from given source.
         * \param   src     The source of data to copy
         **/
        const NERegistry::CEComponentEntry & operator = ( const NERegistry::CEComponentEntry & src );

        /**
         * \brief   Checks equality of two Component Entries and returns true if they are equal.
         *          It compares Master Thread name, Role Name, and pointers of Create and Delete Component functions.
         * \param   other   The Component Entry to compare.
         **/
        bool operator == ( const NERegistry::CEComponentEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentEntry class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Component Entry is valid.
         *          The Entry is valid if Role Name of Component Entry is not empty.
         **/
        bool IsValid( void ) const;

        /**
         * \brief   Adds supported Service Entry in Component Entry object. 
         *          The Service Entry is defining implemented Service Interface,
         *          supported by Component. A Component may have more than one different 
         *          Service Interfaces implementation.
         * \param   entry   The Service Entry, defining implemented 
         *                  Service Interface in Component.
         **/
        void AddSupportedService( const NERegistry::CEServiceEntry & entry );
        
        /**
         * \brief   Adds List of supported Service Entries in Component Entry object.
         *          Every Service Entry in the List is defining implemented Service Interface,
         *          supported by Component. A Component may have more than one different 
         *          Service Interfaces implementation.
         * \param   serviceList The list of Service Entries. Every Service Entry is defining
         *                      implemented Service Interfaces in Component.
         **/
        void AddSupportedService( const NERegistry::CEServiceList & serviceList );

        /**
         * \brief   Searches given supported service. If found, removes from list.
         * \param   serviceName The name of supported service
         * \return  Returns true if found and remove supported service entry in the list.
         **/
        bool RemoveSupportedService( const char * serviceName );

        /**
         * \brief   Searches Service Entry in the existing list of Component Entry
         *          and returns the zero-based valid position if found. 
         *          Otherwise, returns -1, if cannot find Service Entry in the list.
         * \param   entry   The Service Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int SearchSupportedService( const NERegistry::CEServiceEntry & entry ) const;

        /**
         * \brief   By given name, searches Service Entry in the existing list of 
         *          Component Entry and returns the zero-based valid position if found.
         *          Returns -1, if cannot find Service Entry in the list.
         * \param   serviceName The Name of Service Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int SearchSupportedService( const char* serviceName ) const;

        /**
         * \brief   Adds Worker Thread Entry in Component Entry object. 
         *          The Worker Thread Entry is defining Worker Thread object,
         *          binded with Component. A Component may have zero or more
         *          binded Worker Threads.
         * \param   entry   The Worker Thread Entry, defining Worker Thread
         *                  object binded with Component.
         **/
        void AddWorkerThread( const NERegistry::CEWorkerThreadEntry & entry );

        /**
         * \brief   Adds List of Worker Thread Entries in Component Entry object. 
         *          Every Worker Thread Entry in the List is defining Worker Thread object,
         *          binded with Component. A Component may have zero or more
         *          binded Worker Threads.
         * \param   workerList  The List of Worker Thread Entries, defining list of 
         *                      Worker Thread objects binded with Component.
         **/
        void AddWorkerThread( const NERegistry::CEWorkerThreadList & workerList );

        /**
         * \brief   Searches Worker Thread Entry in the existing list of Component Entry
         *          and returns the valid position in the list. Returns -1, if cannot
         *          find Worker Thread Entry in the list.
         * \param   entry   The Worker Thread Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element. 
         *          Otherwise, returns -1.
         **/
        int SearchWorkerThread( const NERegistry::CEWorkerThreadEntry & entry ) const;

        /**
         * \brief   By given name, searches Worker Thread Entry in the existing list of 
         *          Component Entry and returns the valid position in the list. 
         *          Returns -1, if cannot find Worker Thread Entry in the list.
         * \param   workerName  The Name of Worker Thread Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element. 
         *          Otherwise, returns -1.
         **/
        int SearchWorkerThread( const char* workerName ) const;

        /**
         * \brief   Searches the worker thread entry in the list.
         *          If found, removes it from the list.
         * \param   workerName  The name of worker thread to remove
         * \return  Returns true if worker thread entry found and could remove.
         **/
        bool RemoveWorkerThread( const char * workerName );

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
        void AddDependencyService( const NERegistry::CEDependencyEntry & entry );

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
        void AddDependencyService( const NERegistry::CEDependencyList & dependencyList );

        /**
         * \brief   Searches Dependency Entry in the existing list of Component Entry
         *          and returns the valid position in the list. Returns -1, if cannot
         *          find Dependency Entry in the list.
         * \param   entry   The Dependency Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int SearchDependencyService( const NERegistry::CEDependencyEntry & entry ) const;

        /**
         * \brief   Searches Service in dependency service list by given Role Name.
         *          If found, removes entry from the list.
         * \param   serviceName     The name of dependency service role name to search.
         * \return  Returns true if found entry and could remove it.
         **/
        bool RemoveDependencyService( const char * roleName );

        /**
         * \brief   By given name, searches Dependency Entry in the existing list of 
         *          Component Entry and returns the valid position in the list. 
         *          Returns -1, if cannot find Dependency Entry in the list.
         * \param   roleName    The Name of Dependency Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int SearchDependencyService(const char* roleName) const;

        /**
         * \brief   Returns list of supported Services of Component Entry object
         **/
        const NERegistry::CEServiceList & GetSupportedServices( void ) const;

        /**
         * \brief   Returns list of Worker Threads of Component Entry object
         **/
        const NERegistry::CEWorkerThreadList & GetWorkerThreads( void ) const;

        /**
         * \brief   Returns list of Dependencies of Component Entry object
         **/
        const NERegistry::CEDependencyList & GetDependencyServices( void ) const;

        /**
         * \brief   Sets pointers to create and delete component methods to be called when instantiating component object.
         *          If NULL, it clears existing pointers and no component is instantiated.
         * \param   fnCreate    The pointer to create component method.
         * \param   fnDelete    The pointer to delete component method.
         **/
        void SetInstanceMethods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete );

        /**
         * \brief   Sets component data to pass to component create method.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    The system automatically does not free manually allocated space. This means that 
         *          if as a component data a pointer to manually allocated object is passed,
         *          it should be as well manually freed.
         **/
        void SetComponentData( NEMemory::uAlign compData );

        /**
         * \brief   Returns the pointer to component data.
         **/
        NEMemory::uAlign GetComponentData( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   The Role Name of Component
         **/
        CEString            mRoleName;
        /**
         * \brief   The Master Thread Entry of Component.
         **/
        CEString            mThreadName;

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
        CEServiceList       mSupportedServices;

        /**
         * \brief   List of worker threads
         **/
        CEWorkerThreadList  mWorkerThreads;

        /**
         * \brief   List of dependencies
         **/
        CEDependencyList    mDependencyServices;

        /**
         * \brief   The data to pass to component create method.
         * \note    If data is created by allocating memory (for example, via 'new' operator),
         *          it is not automatically deleted in destructor of Component Registry object.
         **/
        mutable NEMemory::uAlign    mComponentData;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentEntryList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEComponentList. Defines list of Component Entries.
     *          It is a list of all Components within one Thread.
     *          All Components should be valid and have unique Role Name.
     *          None of Components should have same implemented Service Interface.
     *          If more than one Components are implementing same Service Interface,
     *          they should be listed in different list of different thread.
     **/
    class AREG_API CEComponentList   : public TEArrayList<NERegistry::CEComponentEntry, const NERegistry::CEComponentEntry&>
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentList class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class CEComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates empty list.
         **/
        CEComponentList( void );

        /**
         * \brief   Creates Component List and adds specified Component Entry to the list as first element.
         * \param   entry   The Component Entry to set as first element in the list.
         **/
        CEComponentList(  const NERegistry::CEComponentEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEComponentList( const NERegistry::CEComponentList & src );

        /**
         * \brief   Destructor
         **/
        virtual ~CEComponentList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentList class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Component Entries from given source of Component List.
         * \param   src     The source of Component List, containing Component Entries.
         **/
        const NERegistry::CEComponentList & operator = ( const NERegistry::CEComponentList & src );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Component List is valid.
         **/
        bool IsValid( void ) const;

        /**
         * \brief   Searches Component Entry having given name. Returns valid
         *          Component Entry if found, otherwise returns invalid entry
         *          NERegistry::INVALID_COMPONENT_ENTRY.
         *          There should not be several Component Entries with same name. 
         * \param   roleName    The name of Component Entry to search.
         * \return  Returns valid Component Entry if found. Otherwise returns
         *          invalid entry NERegistry::INVALID_COMPONENT_ENTRY.
         **/
        const NERegistry::CEComponentEntry & GetComponent( const char* roleName ) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    The system automatically does not free manually allocated space. This means that 
         *          if as a component data a pointer to manually allocated object is passed,
         *          it should be as well manually freed.
         **/
        bool SetComponentData( const char * roleName, NEMemory::uAlign compData );

        /**
         * \brief   Searches Component Entry by given Role Name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   roleName    The Role Name of Component Entry to search.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchComponent( const char* roleName ) const;

        /**
         * \brief   Searches specified Component Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Component Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchComponent(const NERegistry::CEComponentEntry& entry) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEComponentThreadEntry. Defines Master Thread of one or
     *          mode registered Component. All registered Components are 
     *          created in specified thread. One Thread should have at least
     *          one registered Component. All registered Components should have
     *          unique Role Names and different Service Names.
     **/
    class AREG_API CEComponentThreadEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadEntry class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class CEComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates invalid Thread Entry.
         **/
        CEComponentThreadEntry( void );

        /**
         * \brief   Initialize Thread Entry with given Thread Name.
         * \param   threadName  The Thread Name to assign.
         **/
        CEComponentThreadEntry( const char* threadName );

        /**
         * \brief   Initialize Thread Entry with given Thread Name and given Component List.
         * \param   threadName      The Thread Name to assign.
         * \param   componentList   The List of Component Entries to assign.
         **/
        CEComponentThreadEntry( const char* threadName, const NERegistry::CEComponentList & componentList );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEComponentThreadEntry( const NERegistry::CEComponentThreadEntry & src );

        /**
         * \brief   Destructor
         **/
        virtual ~CEComponentThreadEntry( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Thread Entry data from given source.
         * \param   src     The source of data to copy
         **/
        const NERegistry::CEComponentThreadEntry & operator = ( const NERegistry::CEComponentThreadEntry & src );

        /**
         * \brief   Checks equality of two Thread Entries and returns true if they are equal.
         *          It compares Thread names.
         * \param   other   The Thread Entry to compare.
         **/
        bool operator == ( const NERegistry::CEComponentThreadEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadEntry class, Attribute and operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Thread Entry is valid.
         *          The Entry is valid if Name of Thread Entry is not empty.
         **/
        bool IsValid( void ) const;

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
        void AddComponent( const NERegistry::CEComponentEntry & entry );

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
        void AddComponent( const NERegistry::CEComponentList & componentList );

        /**
         * \brief   Searches the component entry by given role name.
         *          If found, remove the component entry from the list.
         * \param   roleName    The roleName of service component to search and remove.
         * \return  Returns true if found and could remove the component entry.
         **/
        bool RemoveComponent( const char * roleName );

        /**
         * \brief   Searches Component Entry in the existing list of Thread Entry
         *          and returns the zero-based valid position if found. 
         *          Otherwise, returns -1, if cannot find Component Entry in the list.
         * \param   entry   The Component Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int SearchComponentEntry( const NERegistry::CEComponentEntry & entry ) const;

        /**
         * \brief   By given name, searches Component Entry in the existing list of 
         *          Thread Entry and returns the zero-based valid position if found.
         *          Returns -1, if cannot find Component Entry in the list.
         * \param   roleName    The Name of Component Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int SearchComponentEntry( const char* roleName ) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    The system automatically does not free manually allocated space. This means that 
         *          if as a component data a pointer to manually allocated object is passed,
         *          it should be as well manually freed.
         **/
        bool SetComponentData( const char * roleName, NEMemory::uAlign compData );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   The name of thread component (thread name)
         **/
        CEString        mThreadName;

        /**
         * \brief   List of component entries
         **/
        CEComponentList mComponents;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEComponentThreadList. Defines list of Thread Entries in Model.
     *          All names of Thread should be unique. A Model has more than one
     *          Thread Entry.
     **/
    class AREG_API CEComponentThreadList  : public TEArrayList< NERegistry::CEComponentThreadEntry, const NERegistry::CEComponentThreadEntry & >
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadList class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class CEComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadList class, Constructors / destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Default constructor.
         **/
        CEComponentThreadList( void );

        /**
         * \brief   Creates Component Thread List and adds specified Thread Entry to the list as first element.
         * \param   entry   The Thread Entry to set as first element in the list.
         **/
        CEComponentThreadList( const NERegistry::CEComponentThreadEntry & entry );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEComponentThreadList( const NERegistry::CEComponentThreadList & src );

        /**
         * \brief   Destructor
         **/
        virtual ~CEComponentThreadList( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadList class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Thread Entries from given source of Thread List.
         * \param   src     The source of Thread List, containing Thread Entries.
         **/
        const NERegistry::CEComponentThreadList & operator = ( const NERegistry::CEComponentThreadList & src );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEComponentThreadList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Thread List is valid.
         **/
        bool IsValid( void ) const;

        /**
         * \brief   Searches Thread Entry having given name. Returns valid
         *          Thread Entry if found, otherwise returns invalid entry
         *          NERegistry::INVALID_THREAD_ENTRY.
         *          There should not be several Thread Entries with same name. 
         * \param   threadName  The name of Thread Entry to search.
         * \return  Returns valid Thread Entry if found. Otherwise returns
         *          invalid entry NERegistry::INVALID_THREAD_ENTRY.
         **/
        const NERegistry::CEComponentThreadEntry & GetThread( const char* threadName ) const;

        /**
         * \brief   Searches Thread Entry by given Thread Name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   threadName  The Thread Name of Thread Entry to search.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchThread( const char* threadName ) const;

        /**
         * \brief   Searches specified Thread Entry and returns 
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Thread Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. 
         *          Otherwise, returns -1.
         **/
        int SearchThread( const NERegistry::CEComponentThreadEntry & entry ) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEModel class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NERegistry::CEModel. Defines Model of project / binary.
     *          Every project / binary can have only one Model, specifying model of
     *          Threads, Components and dependencies.
     *          If several Models should be defined in application, they should
     *          be specified separately in separate projects / binaries.
     **/
    class AREG_API CEModel
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEModel class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class CEComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEModel class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates empty Model.
         **/
        CEModel( void );

        /**
         * \brief   Initialize empty Model and sets given Name.
         **/
        CEModel( const char* modelName );

        /**
         * \brief   Initialize empty Model and sets given Name.
         **/
        CEModel( const char* modelName, const CEComponentThreadList & threadList );

        /**
         * \brief   Copy constructor.
         * \param   src     The source of data to copy.
         **/
        CEModel( const CEModel & src );

        /**
         * \brief   Destructor
         **/
        ~CEModel( void );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEModel class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Model data from given source, 
         * \param   src     The source of data to copy
         **/
        const NERegistry::CEModel & operator = ( const NERegistry::CEModel & src );

        /**
         * \brief   Checks equality of two Models and returns true if they are equal.
         *          It will compare Model names and registered Thread Entries.
         **/
        bool operator == ( const NERegistry::CEModel & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEModel class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true, if Mode is valid.
         *          The Model is valid if Name is not empty and it contains at least one thread entry.
         **/
        bool IsValid( void ) const;

        /**
         * \brief   Returns the name of model.
         **/
        const CEString & GetModelName( void ) const;

        /**
         * \brief   Searches specified Component Entry  and returns true if 
         *          found specified Component Entry. Otherwise returns false.
         * \param   entry   The Component Entry to search in entire Model.
         * \return  Returns true if found specified Component Entry.
         *          Otherwise returns false.
         **/
        bool HasRegisteredComponent( const NERegistry::CEComponentEntry & entry ) const;

        /**
         * \brief   Searches specified Component Entry  and returns true if 
         *          found specified Component Entry by Role Name. Otherwise returns false.
         * \param   roleName    The Role Name of Component Entry to search in entire Model.
         * \return  Returns true if found specified Component Entry.
         *          Otherwise returns false.
         **/
        bool HasRegisteredComponent( const char * roleName ) const;

        /**
         * \brief   Returns true if Model is loaded. Otherwise, returns false.
         **/
        bool IsModelLoaded( void ) const;

        /**
         * \brief   Returns the List of Thread Entries.
         **/
        const NERegistry::CEComponentThreadList & GetThreadList( void ) const;

        /**
         * \brief   Sets flag and marks Model as loaded or unloaded.
         * \param   isLoaded    The flag to set. 
         *                      If true, the Model is marked as loaded.
         *                      If false, the Model is marked as unloaded.
         **/
        void SetModelLoaded( bool isLoaded = true );

        /**
         * \brief   Adds supported Thread Entry to the list of Model.
         *          Every Thread Entry should have global unique Name.
         * \param   entry   The Thread Entry to add.
         **/
        void AddThread( const NERegistry::CEComponentThreadEntry & entry );

        /**
         * \brief   Adds List of supported Thread Entries to the list of Model.
         *          Every Thread Entry in the List should have global unique Name.
         * \param   threadList  The list of Thread Entries to add.
         **/
        void AddThread( const NERegistry::CEComponentThreadList & threadList );

        /**
         * \brief   Searches component thread entry in the list.
         *          If found, removes it.
         * \param   threadName  The name of component thread to search.
         * \return  Returns true if found and could remove the thread entry from the list.
         **/
        bool RemoveThread( const char * threadName );

        /**
         * \brief   Searches Thread Entry in the existing list of Model
         *          and returns the zero-based valid position if found. 
         *          Otherwise, returns -1, if cannot find Thread Entry in Model.
         * \param   entry   The Thread Entry to search in Model.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int SearchThread( const NERegistry::CEComponentThreadEntry & entry ) const;

        /**
         * \brief   By given name, searches Thread Entry in the existing list of 
         *          Model and returns the zero-based valid position if found.
         *          Returns -1, if cannot find Thread Entry in Model.
         * \param   threadName  The Name of Thread Entry to search in Model.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int SearchThread(const char* threadName) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    The system automatically does not free manually allocated space. This means that 
         *          if as a component data a pointer to manually allocated object is passed,
         *          it should be as well manually freed.
         **/
        bool SetComponentData( const char * roleName, NEMemory::uAlign compData );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::CEModel class, Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   The Name of Model
         **/
        CEString                mModelName;

        /**
         * \brief   The List of Thread Entries in Model
         **/
        CEComponentThreadList   mModelThreads;

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
     extern AREG_API const NERegistry::CEServiceEntry           INVALID_SERVICE_ENTRY;

     /**
      * \brief   NERegistry::INVALID_SERVICE_LIST
      *          Predefined invalid Service List.
      **/
    extern AREG_API const NERegistry::CEServiceList             INVALID_SERVICE_LIST;

    /**
     * \brief   NERegistry::INVALID_WORKER_THREAD_ENTRY
     *          Predefined invalid Worker Thread Entry.
     **/
    extern AREG_API const NERegistry::CEWorkerThreadEntry       INVALID_WORKER_THREAD_ENTRY;

    /**
     * \brief   NERegistry::INVALID_WORKER_THREAD_LIST
     *          Predefined invalid Worker Thread List.
     **/
    extern AREG_API const NERegistry::CEWorkerThreadList        INVALID_WORKER_THREAD_LIST;

    /**
     * \brief   NERegistry::INVALID_WORKER_THREAD_LIST
     *          Predefined invalid service dependency entry.
     **/
    extern AREG_API const NERegistry::CEDependencyEntry         INVALID_DEPENDENCY_ENTRY;

    /**
     * \brief   NERegistry::INVALID_WORKER_THREAD_LIST
     *          Predefined invalid service dependency entry.
     **/
    extern AREG_API const NERegistry::CEDependencyList          INVALID_DEPENDENCY_LIST;

    /**
     * \brief   NERegistry::INVALID_COMPONENT_ENTRY
     *          Predefined Invalid Component Entry.
     **/
    extern AREG_API const NERegistry::CEComponentEntry          INVALID_COMPONENT_ENTRY;

    /**
     * \brief   NERegistry::INVALID_COMPONENT_LIST
     *          Predefined invalid Component List.
     **/
    extern AREG_API const NERegistry::CEComponentList           INVALID_COMPONENT_LIST;

    /**
     * \brief   NERegistry::INVALID_THREAD_ENTRY
     *          Predefined invalid Thread Entry.
     **/
    extern AREG_API const NERegistry::CEComponentThreadEntry    INVALID_THREAD_ENTRY;

    /**
     * \brief   NERegistry::INVALID_THREAD_LIST
     *          Predefined invalid Thread List. Contains single invalid NERegistry::INVALID_THREAD_ENTRY entry.
     **/
    extern AREG_API const NERegistry::CEComponentThreadList     INVALID_THREAD_LIST;

    /**
     * \brief   NERegistry::CEModel::INVALID_MODEL
     *          Predefined invalid Model.
     **/
    extern AREG_API const NERegistry::CEModel                   INVALID_MODEL;

}

#endif  // AREG_COMPONENT_NEREGISTRY_HPP
