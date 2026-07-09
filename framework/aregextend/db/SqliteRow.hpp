#ifndef AREG_AREGEXTEND_DB_SQLITEROW_HPP
#define AREG_AREGEXTEND_DB_SQLITEROW_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/db/SqliteRow.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite row object to get results.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"


/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg::ext {
    class SqliteStatement;
} // namespace areg::ext

namespace areg::ext {

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
    SqliteRow() noexcept;

    /**
     * \brief   Constructs a SqliteRow from a SqliteStatement.
     * \param   statement       Reference to the SqliteStatement object.
     **/
    SqliteRow(SqliteStatement& statement) noexcept;

    /**
     * \brief   Constructs a SqliteRow from a raw statement pointer.
     * \param   statement       Pointer to the underlying SQLite statement.
     **/
    SqliteRow(void* statement) noexcept;

    SqliteRow(const SqliteRow& src) noexcept;

    SqliteRow(SqliteRow&& src) noexcept;

    ~SqliteRow() = default;

    SqliteRow& operator = (const SqliteRow& src) noexcept;

    SqliteRow& operator = (SqliteRow&& src) noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Checks if the row is valid (i.e., associated with a statement).
     *
     * \return  True if valid, false otherwise.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Retrieves the integer value of the specified column.
     *
     * \param   column      The 0-based column index.
     * \return  The integer value of the column.
     **/
    [[nodiscard]]
    int32_t as_int(int32_t column) const noexcept;

    /**
     * \brief   Retrieves the 64-bit integer value of the specified column.
     *
     * \param   column      The 0-based column index.
     * \return  The 64-bit integer value of the column.
     **/
    [[nodiscard]]
    int64_t as_int64(int32_t column) const noexcept;

    /**
     * \brief   Retrieves the double value of the specified column.
     *
     * \param   column      The 0-based column index.
     * \return  The double value of the column.
     **/
    [[nodiscard]]
    double as_double(int32_t column) const noexcept;

    /**
     * \brief   Retrieves the text value of the specified column.
     *
     * \param   column      The 0-based column index.
     * \return  The string value of the column.
     **/
    [[nodiscard]]
    String as_text(int32_t column) const noexcept;

    /**
     * \brief   Checks if the specified column is NULL.
     *
     * \param   column      The 0-based column index.
     * \return  True if the column is NULL, false otherwise.
     **/
    [[nodiscard]]
    bool is_null(int32_t column) const noexcept;

    /**
     * \brief   Checks if the specified column index is valid.
     *
     * \param   column      The 0-based column index.
     * \return  True if the column index is valid, false otherwise.
     **/
    [[nodiscard]]
    bool is_column_valid(int32_t column) const noexcept;

    /**
     * \brief   Checks if the specified column contains a string value.
     *
     * \param   column      The 0-based column index.
     * \return  True if the column is a string, false otherwise.
     **/
    [[nodiscard]]
    bool is_string(int32_t column) const noexcept;

    /**
     * \brief   Checks if the specified column contains a 32-bit integer value.
     *
     * \param   column      The 0-based column index.
     * \return  True if the column is a 32-bit integer, false otherwise.
     **/
    [[nodiscard]]
    bool is_integer(int32_t column) const noexcept;

    /**
     * \brief   Checks if the specified column contains a 64-bit integer value.
     *
     * \param   column      The 0-based column index.
     * \return  True if the column is a 64-bit integer, false otherwise.
     **/
    [[nodiscard]]
    bool is_integer64(int32_t column) const noexcept;

    /**
     * \brief   Checks if the specified column contains a double value.
     *
     * \param   column      The 0-based column index.
     * \return  True if the column is a double, false otherwise.
     **/
    [[nodiscard]]
    bool is_double(int32_t column) const noexcept;

    /**
     * \brief   Returns the number of columns in the row.
     *
     * \return  The number of columns.
     **/
    [[nodiscard]]
    int32_t column_count() const noexcept;

    /**
     * \brief   Returns the name of the specified column.
     *
     * \param   column      The 0-based column index.
     * \return  The name of the column.
     **/
    [[nodiscard]]
    String column_name(int32_t column) const noexcept;

    /**
     * \brief   Returns the index of the column with the specified name.
     *
     * \param   columnName      The name of the column.
     * \return  The 0-based index of the column, or -1 if not found.
     **/
    [[nodiscard]]
    int32_t column_index(const String& columnName) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Pointer to the underlying SQLite statement.
     */
    void* mStatement;
};

inline bool SqliteRow::is_valid() const noexcept
{
    return (mStatement != nullptr);
}

} // namespace areg::ext

#endif // AREG_AREGEXTEND_DB_SQLITEROW_HPP
