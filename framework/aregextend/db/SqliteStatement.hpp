#ifndef AREG_AREGEXTEND_DB_SQLITESTATEMENT_HPP
#define AREG_AREGEXTEND_DB_SQLITESTATEMENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/db/SqliteStatement.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite statement object.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
#include "aregextend/db/SqliteRow.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg::ext {
    class SqliteDatabase;
}

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// SqliteStatement class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Prepared SQLite statement with parameter binding, result row iteration, and column value
 *          access.
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
    enum class ColumnType  : uint8_t
    {
          Unknown   = 0 //!< Unknown column type.
        , Integer       //!< 32-bit integer column.
        , Integer64     //!< 64-bit integer column.
        , Double        //!< Double-precision floating point column.
        , Text          //!< Text (string) column.
        , Blob          //!< Binary large object column.
        , Null          //!< Null column.
    };

    /**
     * \brief   Values set when getting the next row in the result set.
     **/
    enum class QueryResult  : uint8_t
    {
          Failed    = 0 //!< The query failed to execute.
        , HasMore   = 1 //!< The query executed successfully and has rows to process.
        , HasNoMore = 2 //!< The query executed successfully and has no more rows to process.
    };

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs and prepares a SqliteStatement with the given SQL.
     *
     * \param   db      Reference to the SqliteDatabase object.
     * \param   sql     The SQL statement to prepare.
     **/
    SqliteStatement(SqliteDatabase& db, const String& sql);

    /**
     * \brief   Constructs a SqliteStatement associated with the database. The statement is not
     *          prepared until prepare() is called.
     *
     * \param   db      Reference to the SqliteDatabase object.
     **/
    SqliteStatement(SqliteDatabase& db) noexcept;

    ~SqliteStatement() noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Prepares the SQL statement for execution.
     *
     * \param   sql     The SQL statement to prepare.
     * \return  True if the statement was prepared successfully; false otherwise.
     **/
    bool prepare(const String& sql);

    /**
     * \brief   Executes the prepared statement. For SELECT queries, use next() to fetch results.
     * \return  True if execution was successful; false otherwise.
     **/
    bool execute() noexcept;

    /**
     * \brief   Advances to the next row in the result set.
     *
     * \return  QueryResult::Failed if an error occurred; QueryResult::HasMore if a row is
     *          available; QueryResult::HasNoMore if the end of the result set is reached.
     **/
    [[nodiscard]]
    SqliteStatement::QueryResult next() noexcept;

    /**
     * \brief   Resets the statement to its initial prepared state, allowing re-execution with new
     *          parameter values.
     **/
    void reset() noexcept;

    /**
     * \brief   Finalizes the statement and releases associated resources.
     **/
    void finalize() noexcept;

    /**
     * \brief   Checks if the statement is prepared and ready for execution.
     *
     * \return  True if the statement is prepared; false otherwise.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns the current row position in the result set. The first row is 1; returns 0 if
     *          the statement is not prepared.
     **/
    [[nodiscard]]
    inline uint32_t row_pos() const noexcept;

    /**
     * \brief   Binds a 32-bit integer value to the specified parameter.
     *
     * \param   index       The 0-based parameter index.
     * \param   value       The integer value to bind.
     * \return  True if binding was successful; false otherwise.
     **/
    bool bind_int32(int32_t index, int32_t value) noexcept;

    /**
     * \brief   Binds a 32-bit unsigned integer value to the specified parameter.
     *
     * \param   index       The 0-based parameter index.
     * \param   value       The unsigned integer value to bind.
     * \return  True if binding was successful; false otherwise.
     **/
    bool bind_uint32(int32_t index, uint32_t value) noexcept;

    /**
     * \brief   Binds a 64-bit integer value to the specified parameter.
     *
     * \param   index       The 0-based parameter index.
     * \param   value       The 64-bit integer value to bind.
     * \return  True if binding was successful; false otherwise.
     **/
    bool bind_int64(int32_t index, int64_t value) noexcept;

    /**
     * \brief   Binds a 64-bit unsigned integer value to the specified parameter.
     *
     * \param   index       The 0-based parameter index.
     * \param   value       The 64-bit unsigned integer value to bind.
     * \return  True if binding was successful; false otherwise.
     **/
    bool bind_uint64(int32_t index, uint64_t value) noexcept;

    /**
     * \brief   Binds a double value to the specified parameter.
     *
     * \param   index       The 0-based parameter index.
     * \param   value       The double value to bind.
     * \return  True if binding was successful; false otherwise.
     **/
    bool bind_double(int32_t index, double value) noexcept;

    /**
     * \brief   Binds a float value to the specified parameter.
     *
     * \param   index       The 0-based parameter index.
     * \param   value       The float value to bind.
     * \return  True if binding was successful; false otherwise.
     **/
    bool bind_float(int32_t index, float value) noexcept;

    /**
     * \brief   Binds a text value to the specified parameter.
     *
     * \param   index       The 0-based parameter index.
     * \param   value       The string value to bind.
     * \return  True if binding was successful; false otherwise.
     **/
    bool bind_text(int32_t index, const String& value) noexcept;

    /**
     * \brief   Binds a NULL value to the specified parameter.
     *
     * \param   index       The 0-based parameter index.
     * \return  True if binding was successful; false otherwise.
     **/
    bool bind_null(int32_t index) noexcept;

    /**
     * \brief   Clears all parameter bindings for the prepared statement.
     **/
    void clear_bindings() noexcept;

    /**
     * \brief   Retrieves the 32-bit integer value of the specified column in the current row.
     *
     * \param   index       The 0-based column index.
     * \return  The integer value of the column.
     **/
    [[nodiscard]]
    int32_t as_int32(int32_t index) const noexcept;

    /**
     * \brief   Retrieves the 32-bit unsigned integer value of the specified column in the current row.
     *
     * \param   index       The 0-based column index.
     * \return  The unsigned integer value of the column.
     **/
    [[nodiscard]]
    uint32_t as_uint32(int32_t index) const noexcept;

    /**
     * \brief   Retrieves the 64-bit integer value of the specified column in the current row.
     *
     * \param   index       The 0-based column index.
     * \return  The 64-bit integer value of the column.
     **/
    [[nodiscard]]
    int64_t as_int64(int32_t index) const noexcept;

    /**
     * \brief   Retrieves the 64-bit unsigned integer value of the specified column in the current row.
     *
     * \param   index       The 0-based column index.
     * \return  The 64-bit unsigned integer value of the column.
     **/
    [[nodiscard]]
    uint64_t as_uint64(int32_t index) const noexcept;

    /**
     * \brief   Retrieves the double value of the specified column in the current row.
     *
     * \param   index       The 0-based column index.
     * \return  The double value of the column.
     **/
    [[nodiscard]]
    double as_double(int32_t index) const noexcept;
    /**
     * \brief   Retrieves the float value of the specified column in the current row.
     *
     * \param   index       The 0-based column index.
     * \return  The float value of the column.
     **/
    [[nodiscard]]
    float as_float(int32_t index) const noexcept;

    /**
     * \brief   Retrieves the text value of the specified column in the current row.
     *
     * \param   index       The 0-based column index.
     * \return  The string value of the column.
     **/
    [[nodiscard]]
    String as_text(int32_t index) const;

    /**
     * \brief   Checks if the specified column in the current row is NULL.
     *
     * \param   column  The 0-based column index.
     * \return  True if the column is NULL; false otherwise.
     **/
    [[nodiscard]]
    bool is_null(int32_t column) const noexcept;

    /**
     * \brief   Checks if the specified column index is valid for the current result set.
     *
     * \param   index       The 0-based column index.
     * \return  True if the column index is valid; false otherwise.
     **/
    [[nodiscard]]
    bool is_column_valid(int32_t index) const noexcept;

    /**
     * \brief   Checks if the specified column contains a string value.
     *
     * \param   index       The 0-based column index.
     * \return  True if the column is a string; false otherwise.
     **/
    [[nodiscard]]
    bool is_string(int32_t index) const noexcept;

    /**
     * \brief   Checks if the specified column contains a 32-bit integer value.
     *
     * \param   index       The 0-based column index.
     * \return  True if the column is a 32-bit integer; false otherwise.
     **/
    [[nodiscard]]
    bool is_int(int32_t index) const noexcept;

    /**
     * \brief   Checks if the specified column contains a 64-bit integer value.
     *
     * \param   index       The 0-based column index.
     * \return  True if the column is a 64-bit integer; false otherwise.
     **/
    [[nodiscard]]
    bool is_int64(int32_t index) const noexcept;

    /**
     * \brief   Checks if the specified column contains a double value.
     *
     * \param   index       The 0-based column index.
     * \return  True if the column is a double; false otherwise.
     **/
    [[nodiscard]]
    bool is_double(int32_t index) const noexcept;

    /**
     * \brief   Returns the number of columns in the result set.
     *
     * \return  The number of columns.
     **/
    [[nodiscard]]
    int32_t column_count() const noexcept;

    /**
     * \brief   Returns the name of the specified column.
     *
     * \param   index       The 0-based column index.
     * \return  The name of the column.
     **/
    [[nodiscard]]
    String column_name(int32_t index) const;

    /**
     * \brief   Returns the index of the column with the specified name.
     *
     * \param   columnName      The name of the column.
     * \return  The 0-based index of the column, or -1 if not found.
     **/
    [[nodiscard]]
    int32_t column_index(const String& columnName) const noexcept;

    /**
     * \brief   Returns the type of the specified column.
     *
     * \param   index       The 0-based column index.
     * \return  The column type as ColumnType.
     **/
    [[nodiscard]]
    SqliteStatement::ColumnType column_type(int32_t index) const noexcept;

    /**
     * \brief   Returns a SqliteRow object representing the current row.
     *
     * \return  The current SqliteRow.
     **/
    [[nodiscard]]
    SqliteRow row() const noexcept;

    /**
     * \brief   Advances to the next row and returns a SqliteRow object for it.
     *
     * \return  The next SqliteRow.
     **/
    [[nodiscard]]
    SqliteRow next_row() const noexcept;

    /**
     * \brief   Advances to the next row in the result set (const overload).
     *
     * \return  True if a new row is available; false otherwise.
     **/
    bool next() const noexcept;

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
    SqliteStatement() = delete; //!< Default constructor is not allowed.
    AREG_NOCOPY_NOMOVE(SqliteStatement); //!< No copy or move allowed.
};

inline bool SqliteStatement::is_valid() const noexcept
{
    return (mStatement != nullptr);
}


inline uint32_t SqliteStatement::row_pos() const noexcept
{
    return mRowPos;
}

} // namespace areg::ext

#endif // AREG_AREGEXTEND_DB_SQLITESTATEMENT_HPP
