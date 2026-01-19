#ifndef AREG_AREGEXTEND_DB_SQLITEROW_HPP
#define AREG_AREGEXTEND_DB_SQLITEROW_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/db/SqliteRow.hpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite row object to get results.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class SqliteStatement;

//////////////////////////////////////////////////////////////////////////
// SqliteRow class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Represents a single row of results from a SQLite query.
 *          Provides access to column values and metadata for the current row.
 **/
class SqliteRow
{
//////////////////////////////////////////////////////////////////////////
// Constructors / operators.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Initializes an invalid row.
     */
    SqliteRow(void);

    /**
     * \brief   Constructs a SqliteRow from a SqliteStatement.
     * \param   statement   Reference to the SqliteStatement object.
     */
    SqliteRow(SqliteStatement& statement);

    /**
     * \brief   Constructs a SqliteRow from a raw statement pointer.
     * \param   statement   Pointer to the underlying SQLite statement.
     */
    SqliteRow(void* statement);

    /**
     * \brief   Copy constructor.
     * \param   src     The source SqliteRow to copy from.
     */
    SqliteRow(const SqliteRow& src);

    /**
     * \brief   Move constructor.
     * \param   src     The source SqliteRow to move from.
     */
    SqliteRow(SqliteRow&& src) noexcept;

    /**
     * \brief   Destructor. Defaulted.
     */
    ~SqliteRow(void) = default;

    /**
     * \brief   Copy assignment operator.
     * \param   src     The source SqliteRow to copy from.
     * \return  Reference to this object.
     */
    SqliteRow& operator = (const SqliteRow& src);

    /**
     * \brief   Move assignment operator.
     * \param   src     The source SqliteRow to move from.
     * \return  Reference to this object.
     */
    SqliteRow& operator = (SqliteRow&& src) noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Checks if the row is valid (i.e., associated with a statement).
     * \return  True if valid, false otherwise.
     */
    inline bool isValid(void) const;

    /**
     * \brief   Retrieves the integer value of the specified column.
     * \param   column  The 0-based column index.
     * \return  The integer value of the column.
     */
    int getInt(int column) const;

    /**
     * \brief   Retrieves the 64-bit integer value of the specified column.
     * \param   column  The 0-based column index.
     * \return  The 64-bit integer value of the column.
     */
    int64_t getInt64(int column) const;

    /**
     * \brief   Retrieves the double value of the specified column.
     * \param   column  The 0-based column index.
     * \return  The double value of the column.
     */
    double getDouble(int column) const;

    /**
     * \brief   Retrieves the text value of the specified column.
     * \param   column  The 0-based column index.
     * \return  The string value of the column.
     */
    String getText(int column) const;

    /**
     * \brief   Checks if the specified column is NULL.
     * \param   column  The 0-based column index.
     * \return  True if the column is NULL, false otherwise.
     */
    bool isNull(int column) const;

    /**
     * \brief   Checks if the specified column index is valid.
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
     * \brief   Returns the number of columns in the row.
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

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Pointer to the underlying SQLite statement.
     */
    void* mStatement;
};

inline bool SqliteRow::isValid(void) const
{
    return (mStatement != nullptr);
}

#endif // AREG_AREGEXTEND_DB_SQLITEROW_HPP
