#ifndef AREG_BASE_NEBASICSHAPE_HPP
#define AREG_BASE_NEBASICSHAPE_HPP
/************************************************************************
 * \file        areg/base/NEBasicShape.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, 
 *              Basic shapes and structures like Point, Size, Rectangle
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"

/************************************************************************
 * Declared classes
 ************************************************************************/
namespace NEBasicShape
{
    struct S_Point;
        class Point;

    struct S_Size;
        class Size;

    struct S_Rect;
        class Rect;
}

//////////////////////////////////////////////////////////////////////////
// NEBasicShape namespace deceleration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Declaration of simple geometrical shapes like
 *          Point, Size and Rectangle, and all simple basic operations
 *          All these objects are declared as a structures, to have fixed
 *          allocation, and classes, to support simple operations.

 *          The following objects are described in this namespace:
 *              1. Point:   is defining simple point with X and Y coordinates;
 *              2. Size:    is defining simple size object with Width and Height
 *              3. Rect:    is defining simple rectangle with top-left X and Y
 *                          coordinates, and Width and Height
 **/
namespace NEBasicShape
{
    //////////////////////////////////////////////////////////////////////////
    // NEBasicShape::sPoint structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NEBasicShape::sPoint
     *          Defines point object with X and Y coordinates
     **/
    typedef struct S_Point 
    {
        int     x;      //!< X-coordinate of point
        int     y;      //!< Y-coordinate of point
    } sPoint;


    //////////////////////////////////////////////////////////////////////////
    // NEBasicShape::sPoint structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NEBasicShape::sSize
     *          Defines size object with Width and Height
     **/
    typedef struct S_Size
    {
        int     width;  //!< Width of size
        int     height; //!< Height of size
    } sSize;

    //////////////////////////////////////////////////////////////////////////
    // NEBasicShape::sPoint structure declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   NEBasicShape::sRect
     *          Defines rectangle object with top-left X- and Y- coordinates,
     *          and Width and Height
     **/
    typedef struct S_Rect
    {
        int     x;      //!< X-coordinate of top-left point of rectangle
        int     y;      //!< Y-coordinate of top-left point of rectangle
        int     width;  //!< Width of rectangle
        int     height; //!< Height of rectangle
    } sRect;

    //////////////////////////////////////////////////////////////////////////
    // NEBasicShape::Point class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Point class object, derived from NEBasicShape::sPoint
     *          Contains simple operations
     **/
    class AREG_API Point  : public NEBasicShape::sPoint
    {
    //////////////////////////////////////////////////////////////////////////
    // NEBasicShape::Point Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        Point( void );
        /**
         * \brief   Initialization constructor. 
         *          Sets passed X and Y values.
         * \param   x   The initial X value of the Point object
         * \param   y   The initial Y value of the Point object
         **/
        Point( int x, int y );
        /**
         * \brief   Initialization constructor.
         *          Sets X and Y values passed by NEBasicShape::sPoint object
         * \param   src     The source of X and Y values taken from NEBasicShape::sPoint object
         **/
        Point( const NEBasicShape::sPoint & src );
        /**
         * \brief   Copy constructor.
         * \param   src     The source to copy data.
         **/
        Point( const Point & src );
        /**
         * \brief   Initialization constructor.
         *          Initializes X and Y values from streaming object.
         * \param   stream  The read-only streaming object containing X and Y values.
         **/
        Point( const IEInStream & stream );
        /**
         * \brief   Destructor
         **/
        ~Point( void);

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // Basic operators
    /************************************************************************/

        /**
         * \brief   Assigning operator. Copies X and Y values from given source
         *          and passed as NEBasicShape::sPoint object.
         * \param   src     The source of X and Y value containing in NEBasicShape::sPoint object
         **/
        const NEBasicShape::Point & operator = ( const NEBasicShape::sPoint & src );
        /**
         * \brief   Compares X and Y values as NEBasicShape::sPoint object
         *          and returns true if they are equal.
         * \param   other   The NEBasicShape::sPoint object to compare
         **/
        inline bool operator == ( const NEBasicShape::sPoint & other ) const;
        /**
         * \brief   Compares X and Y values as NEBasicShape::sPoint object
         *          and returns true if they are not equal.
         * \param   other   The NEBasicShape::sPoint object to compare
         **/
        inline bool operator != ( const NEBasicShape::sPoint & other ) const;

    /************************************************************************/
    // Converting operators
    /************************************************************************/

        /**
         * \brief   Converts NEBasicShape::Point object and returns pointer to const sPoint structure object
         **/
        inline operator const NEBasicShape::sPoint * ( void ) const;

        /**
         * \brief   Converts NEBasicShape::Point object and returns pointer to sPoint structure object
         **/
        inline operator NEBasicShape::sPoint * ( void );

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Converts and returns const NEBasicShape::sPoint type
         *          of NEBasicShape::Point object
         **/
        const NEBasicShape::sPoint & getPoint( void ) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NEBasicShape::Size declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Sizeo class object, derived from NEBasicShape::sSize
     *          Contains simple operations
     **/
    class AREG_API Size   : public NEBasicShape::sSize
    {
    public:
        /**
         * \brief   Default constructor.
         **/
        Size( void );
        /**
         * \brief   Initialization constructor. 
         *          Sets passed Width and Height values.
         * \param   width   The initial Width value of the Size object
         * \param   height  The initial Height value of the Size object
         **/
        Size(int width, int height);
        /**
         * \brief   Initialization constructor.
         *          Sets Width and Height values passed by NEBasicShape::sSize object
         * \param   src     The source of Width and Height values taken from NEBasicShape::sSize object
         **/
        Size( const NEBasicShape::sSize & src );
        /**
         * \brief   Copy constructor.
         * \param   src     The source to copy data.
         **/
        Size( const Size & src );
        /**
         * \brief   Initialization constructor.
         *          Initializes Width and Height values from streaming object.
         * \param   stream  The read-only streaming object containing Width and Height values.
         **/
        Size( const IEInStream & stream );
        /**
         * \brief   Destructor
         **/
        ~Size( void );

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // Basic operators
    /************************************************************************/

        /**
         * \brief   Assigning operator. Copies Width and Height values from given source
         *          and passed as NEBasicShape::sSize object.
         * \param   src     The source of Width and Height value containing in NEBasicShape::sSize object
         **/
        const Size & operator = ( const NEBasicShape::sSize & src );
        /**
         * \brief   Compares Width and Height values as NEBasicShape::sSize object
         *          and returns true if they are equal.
         * \param   other   The NEBasicShape::sSize object to compare
         **/
        inline bool operator == ( const NEBasicShape::sSize & other ) const;
        /**
         * \brief   Compares Width and Height values as NEBasicShape::sSize object
         *          and returns true if they are not equal.
         * \param   other   The NEBasicShape::sSize object to compare
         **/
        inline bool operator != ( const NEBasicShape::sSize & other ) const;

    /************************************************************************/
    // Converting operators
    /************************************************************************/

        /**
         * \brief   Converts NEBasicShape::Size object and returns pointer to const sSize structure object
         **/
        inline operator const NEBasicShape::sSize * ( void ) const;
        /**
         * \brief   Converts NEBasicShape::Size object and returns pointer to const sSize structure object
         **/
        inline operator NEBasicShape::sSize * ( void );

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Converts and returns const NEBasicShape::sSize type
         *          of NEBasicShape::Size object
         **/
        const NEBasicShape::sSize & getSize( void ) const;
    };

    //////////////////////////////////////////////////////////////////////////
    // NEBasicShape::Rect declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Sizeo class object, derived from NEBasicShape::sRect
     *          Contains simple operations
     **/
    class AREG_API Rect   : public NEBasicShape::sRect
    {
    public:
        /**
         * \brief   Default constructor.
         **/
        Rect( void );
        /**
         * \brief   Initialization constructor. 
         *          Sets passed top-left X and Y, and Width and Height values.
         * \param   x       The initial X value of the top-let point of Rect object
         * \param   y       The initial Y value of the top-let point of Rect object
         * \param   width   The initial Width value of the Rect object
         * \param   height  The initial Height value of the Rect object
         **/
        Rect( int x, int y, int width, int height );
        /**
         * \brief   Initialization constructor.
         *          Sets top-left X and Y, and Width and Height values passed by NEBasicShape::sRect object
         * \param   src     The source of top-left X and Y, and Width and Height values taken from NEBasicShape::sRect object
         **/
        Rect( const NEBasicShape::sRect & src );
        /**
         * \brief   Copy constructor.
         * \param   src     The source to copy data.
         **/
        Rect( const Rect & src );
        /**
         * \brief   Initialization constructor.
         *          Initializes top-left X and Y, and Width and Height values from streaming object.
         * \param   stream  The read-only streaming object containing top-left X and Y, and Width and Height values.
         **/
        Rect( const IEInStream & stream );
        /**
         * \brief   Destructor
         **/
        ~Rect( void );

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // Basic operators
    /************************************************************************/

        /**
         * \brief   Assigning operator. Copies top-left X and Y, and Width and Height values from given source
         *          and passed as NEBasicShape::sRect object.
         * \param   src     The source of top-left X and Y, and Width and Height value containing in NEBasicShape::sRect object
         **/
        const Rect & operator = ( const NEBasicShape::sRect & src );
        /**
         * \brief   Compares top-left X and Y, and Width and Height values as NEBasicShape::sRect object
         *          and returns true if they are equal.
         * \param   other   The NEBasicShape::sRect object to compare
         **/
        inline bool operator == (const NEBasicShape::sRect& other) const;
        /**
         * \brief   Compares top-left X and Y, and Width and Height values as NEBasicShape::sRect object
         *          and returns true if they are not equal.
         * \param   other   The NEBasicShape::sRect object to compare
         **/
        inline bool operator != (const NEBasicShape::sRect& other) const;


    /************************************************************************/
    // Converting operators
    /************************************************************************/

        /**
         * \brief   Converts NEBasicShape::Rect object and returns pointer to const sRect structure object
         **/
        inline operator const NEBasicShape::sRect * ( void ) const;
        /**
         * \brief   Converts NEBasicShape::Rect object and returns pointer to sRect structure object
         **/
        inline operator NEBasicShape::sRect * ( void );

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Converts and returns const NEBasicShape::sRect type
         *          of NEBasicShape::Rect object
         **/
        const NEBasicShape::sRect & getRect( void ) const;
    };
}

//////////////////////////////////////////////////////////////////////////
// 
// Inline functions
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Declare following objects streamable:
//  1. NEBasicShape::sPoint
//  2. NEBasicShape::sSize
//  3. NEBasicShape::sRect
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_STREAMABLE(NEBasicShape::sPoint)
IMPLEMENT_STREAMABLE(NEBasicShape::sSize)
IMPLEMENT_STREAMABLE(NEBasicShape::sRect)

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::Point class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline bool NEBasicShape::Point::operator == ( const NEBasicShape::sPoint & other ) const
{
    return (static_cast<const NEBasicShape::sPoint *>(this) != &other ? this->x == other.x && this->y == other.y : true);
}

inline bool NEBasicShape::Point::operator != ( const NEBasicShape::sPoint & other ) const
{
    return (static_cast<const NEBasicShape::sPoint *>(this) != &other ? this->x != other.x || this->y != other.y : false);
}

inline NEBasicShape::Point::operator const NEBasicShape::sPoint * ( void ) const
{
    return static_cast<const NEBasicShape::sPoint *>(this);
}

inline NEBasicShape::Point::operator NEBasicShape::sPoint * ( void )
{
    return static_cast<NEBasicShape::sPoint *>(this);
}

inline const NEBasicShape::sPoint & NEBasicShape::Point::getPoint( void ) const
{
    return static_cast<const NEBasicShape::sPoint &>(*this);
}

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::Size class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline bool NEBasicShape::Size::operator == ( const NEBasicShape::sSize& other ) const
{
    return (static_cast<const NEBasicShape::sSize *>(this) != &other ? this->width == other.width && this->height == other.height : true);
}

inline bool NEBasicShape::Size::operator != ( const NEBasicShape::sSize& other ) const
{
    return (static_cast<const NEBasicShape::sSize *>(this) != &other ? this->width != other.width || this->height != other.height : false);
}

inline NEBasicShape::Size::operator const NEBasicShape::sSize * ( void ) const
{
    return static_cast<const NEBasicShape::sSize *>(this);
}

inline NEBasicShape::Size::operator NEBasicShape::sSize * ( void )
{
    return static_cast<NEBasicShape::sSize *>(this);
}

inline const NEBasicShape::sSize & NEBasicShape::Size::getSize( void ) const
{
    return static_cast<const NEBasicShape::sSize &>(*this);
}

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::Rect class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline bool NEBasicShape::Rect::operator == ( const NEBasicShape::sRect & other ) const
{
    return (static_cast<const NEBasicShape::sRect *>(this) != &other ? this->x == other.x && this->y == other.y && this->width == other.width && this->height == other.height : true);
}

inline bool NEBasicShape::Rect::operator != ( const NEBasicShape::sRect & other ) const
{
    return (static_cast<const NEBasicShape::sRect *>(this) != &other ? this->x == other.x || this->y == other.y || this->width != other.width || this->height != other.height : false);
}

inline NEBasicShape::Rect::operator const NEBasicShape::sRect * ( void ) const
{
    return static_cast<const NEBasicShape::sRect *>(this);
}

inline NEBasicShape::Rect::operator NEBasicShape::sRect * ( void )
{
    return static_cast<NEBasicShape::sRect *>(this);
}

inline const NEBasicShape::sRect & NEBasicShape::Rect::getRect( void ) const
{
    return static_cast<const NEBasicShape::sRect &>(*this);
}

#endif  // AREG_BASE_NEBASICSHAPE_HPP
