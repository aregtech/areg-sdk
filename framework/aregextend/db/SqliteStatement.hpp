#ifndef AREG_AREGEXTEND_DB_SQLITESTATEMENT_HPP
#define AREG_AREGEXTEND_DB_SQLITESTATEMENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/db/SqliteStatement.hpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite statement object.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "aregextend/db/SqliteRow.hpp"

class SqliteDatabase;

//////////////////////////////////////////////////////////////////////////
// SqliteStatement class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * /brief   This class represents a prepared SQL statement that can be executed against a SqliteDatabase.
 **/
class SqliteStatement
{
    friend class SqliteRow;

//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Enum representing the type of a column in the result set.
     */
    enum class eColumnType
    {
          ColumnUnknown     = 0   //!< Unknown column type.
        , ColumnInteger           //!< 32-bit integer column.
        , ColumnInteger64         //!< 64-bit integer column.
        , ColumnDouble            //!< Double-precision floating point column.
        , ColumnText              //!< Text (string) column.
        , ColumnBlob              //!< Binary large object column.
        , ColumnNull              //!< Null column.
    };

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs a SqliteStatement and prepares the given SQL statement.
     * \param   db      Reference to the SqliteDatabase object.
     * \param   sql     The SQL statement to prepare.
     */
    SqliteStatement(SqliteDatabase& db, const String& sql);

    /**
     * \brief   Constructs a SqliteStatement associated with the given database.
     *          The statement is not prepared until prepare() is called.
     * \param   db      Reference to the SqliteDatabase object.
     */
    SqliteStatement(SqliteDatabase& db);

    /**
     * \brief   Destructor. Finalizes the statement and releases resources.
     */
    ~SqliteStatement(void);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Prepares the SQL statement for execution.
     * \param   sql     The SQL statement to prepare.
     * \return  True if the statement was prepared successfully, false otherwise.
     */
    bool prepare(const String& sql);

    /**
     * \brief   Executes the prepared statement (for non-SELECT statements).
     * \return  True if execution was successful, false otherwise.
     */
    bool execute(void);

    /**
     * \brief   Advances to the next row in the result set.
     * \return  True if a new row is available, false if there are no more rows.
     */
    bool next(void);

    /**
     * \brief   Resets the statement to its initial state, ready for re-execution.
     */
    void reset(void);

    /**
     * \brief   Finalizes the statement and releases associated resources.
     */
    void finalize(void);

    /**
     * \brief   Checks if the statement is prepared and ready for execution.
     * \return  True if the statement is prepared, false otherwise.
     */
    inline bool isValid(void) const;

    /**
     * \brief   Returns the current row position in the result set.
     *          The first row starts with 1. Return 0 if SQL statement is not prepared yet.
     **/
    inline uint32_t getRowPos(void) const;

    /**
     * \brief   Binds an integer value to the specified parameter index.
     * \param   index   The 1-based parameter index.
     * \param   value   The integer value to bind.
     * \return  True if binding was successful, false otherwise.
     */
    bool bindInt(int index, int value);

    /**
     * \brief   Binds a 64-bit integer value to the specified parameter index.
     * \param   index   The 1-based parameter index.
     * \param   value   The 64-bit integer value to bind.
     * \return  True if binding was successful, false otherwise.
     */
    bool bindInt64(int index, int64_t value);

    /**
     * \brief   Binds a double value to the specified parameter index.
     * \param   index   The 1-based parameter index.
     * \param   value   The double value to bind.
     * \return  True if binding was successful, false otherwise.
     */
    bool bindDouble(int index, double value);

    /**
     * \brief   Binds a text value to the specified parameter index.
     * \param   index   The 1-based parameter index.
     * \param   value   The string value to bind.
     * \return  True if binding was successful, false otherwise.
     */
    bool bindText(int index, const String& value);

    /**
     * \brief   Binds a NULL value to the specified parameter index.
     * \param   index   The 1-based parameter index.
     * \return  True if binding was successful, false otherwise.
     */
    bool bindNull(int index);

    /**
     * \brief   Clears all parameter bindings for the prepared statement.
     */
    void clearBindings(void);

    /**
     * \brief   Retrieves the integer value of the specified column in the current row.
     * \param   column  The 0-based column index.
     * \return  The integer value of the column.
     */
    int getInt(int column) const;

    /**
     * \brief   Retrieves the 64-bit integer value of the specified column in the current row.
     * \param   column  The 0-based column index.
     * \return  The 64-bit integer value of the column.
     */
    int64_t getInt64(int column) const;

    /**
     * \brief   Retrieves the double value of the specified column in the current row.
     * \param   column  The 0-based column index.
     * \return  The double value of the column.
     */
    double getDouble(int column) const;

    /**
     * \brief   Retrieves the text value of the specified column in the current row.
     * \param   column  The 0-based column index.
     * \return  The string value of the column.
     */
    String getText(int column) const;

    /**
     * \brief   Checks if the specified column in the current row is NULL.
     * \param   column  The 0-based column index.
     * \return  True if the column is NULL, false otherwise.
     */
    bool isNull(int column) const;

    /**
     * \brief   Checks if the specified column index is valid for the current result set.
     * \param   column  The 0-based column index.
     * \return  True if the column index is valid, false otherwise.
     */
    bool isColumnValid(int column) const;

    /**
     * \brief   Checks if the specified column contains a string value.
     * \param   column  The 0-based column index.
     * \return  True if the column is a string, false otherwise.
     */
    bool isString(int column) const;

    /**
     * \brief   Checks if the specified column contains a 32-bit integer value.
     * \param   column  The 0-based column index.
     * \return  True if the column is a 32-bit integer, false otherwise.
     */
    bool isInteger(int column) const;

    /**
     * \brief   Checks if the specified column contains a 64-bit integer value.
     * \param   column  The 0-based column index.
     * \return  True if the column is a 64-bit integer, false otherwise.
     */
    bool isInteger64(int column) const;

    /**
     * \brief   Checks if the specified column contains a double value.
     * \param   column  The 0-based column index.
     * \return  True if the column is a double, false otherwise.
     */
    bool isDouble(int column) const;

    /**
     * \brief   Returns the number of columns in the result set.
     * \return  The number of columns.
     */
    int getColumnCount(void) const;

    /**
     * \brief   Returns the name of the specified column.
     * \param   column  The 0-based column index.
     * \return  The name of the column.
     */
    String getColumnName(int column) const;

    /**
     * \brief   Returns the index of the column with the specified name.
     * \param   columnName The name of the column.
     * \return  The 0-based index of the column, or -1 if not found.
     */
    int getColumnIndex(const String& columnName) const;

    /**
     * \brief   Returns the type of the specified column.
     * \param   column  The 0-based column index.
     * \return  The column type as eColumnType.
     */
    SqliteStatement::eColumnType getColumnType(int column) const;

    /**
     * \brief   Returns a SqliteRow object representing the current row.
     * \return  The current SqliteRow.
     */
    SqliteRow row() const;

    /**
     * \brief   Advances to the next row and returns a SqliteRow object for it.
     * \return  The next SqliteRow.
     */
    SqliteRow nextRow(void) const;

    /**
     * \brief   Advances to the next row in the result set (const overload).
     * \return  True if a new row is available, false otherwise.
     */
    bool next(void) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    SqliteDatabase& mDatabase;  //!< The SQLite database object, which owns this statement.
    void *          mStatement; //!< The SQLite statement object.
    uint32_t        mRowPos;    //!< The current row position.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    SqliteStatement(void) = delete; //!< Default constructor is not allowed.
    DECLARE_NOCOPY_NOMOVE(SqliteStatement); //!< No copy or move allowed.
};

inline bool SqliteStatement::isValid(void) const
{
    return (mStatement != nullptr);
}


inline uint32_t SqliteStatement::getRowPos(void) const
{
    return mRowPos;
}

#endif // AREG_AREGEXTEND_DB_SQLITESTATEMENT_HPP
