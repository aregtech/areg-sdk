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
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/db/SqliteStatement.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite statement object.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"
#include "aregextend/db/SqliteRow.hpp"

namespace aregext { class SqliteDatabase; }
namespace aregext { class SqliteRow; }

//////////////////////////////////////////////////////////////////////////
// SqliteStatement class declaration
//////////////////////////////////////////////////////////////////////////

namespace aregext
{
    /**
     * /brief   This class represents a prepared SQL statement that can be executed against a SqliteDatabase.
     **/
    class SqliteStatement
    {
        friend class aregext::SqliteRow;

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
            , HasNoMore = 2 //<! The query executed successfully and has no more rows to process.
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
        SqliteStatement(aregext::SqliteDatabase& db, const areg::String& sql);

        /**
         * \brief   Constructs a SqliteStatement associated with the given database.
         *          The statement is not prepared until prepare() is called.
         * \param   db      Reference to the SqliteDatabase object.
         */
        SqliteStatement(aregext::SqliteDatabase& db);

        /**
         * \brief   Destructor. Finalizes the statement and releases resources.
         */
        ~SqliteStatement();

    //////////////////////////////////////////////////////////////////////////
    // Attributes and operations
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Prepares the SQL statement for execution.
         * \param   sql     The SQL statement to prepare.
         * \return  True if the statement was prepared successfully, false otherwise.
         */
        bool prepare(const areg::String& sql);

        /**
         * \brief   Executes the prepared statement (for non-SELECT statements).
         * \return  True if execution was successful, false otherwise.
         */
        bool execute();

        /**
         * \brief   Advances to the next row in the result set.
         * \return  One of the results to execute the query.
         *              - The returned value `QueryResult::Failed` means error.
         *              - The returned value `QueryResult::HasMore` means there is data in the row to extract.
         *              - The returned value `QueryResult::HasNoMore` means all data are extracted, reached the end and there is no more data to extract.
         **/
        SqliteStatement::QueryResult next();

        /**
         * \brief   Resets the statement to its initial state, ready for re-execution.
         */
        void reset();

        /**
         * \brief   Finalizes the statement and releases associated resources.
         */
        void finalize();

        /**
         * \brief   Checks if the statement is prepared and ready for execution.
         * \return  True if the statement is prepared, false otherwise.
         */
        inline bool isValid() const;

        /**
         * \brief   Returns the current row position in the result set.
         *          The first row starts with 1. Return 0 if SQL statement is not prepared yet.
         **/
        inline uint32_t getRowPos() const;

        /**
         * \brief   Binds an integer value to the specified parameter index.
         * \param   index   The 0-based parameter index.
         * \param   value   The integer value to bind.
         * \return  True if binding was successful, false otherwise.
         */
        bool bindInt32(int32_t index, int32_t value);
        bool bindUint32(int32_t index, uint32_t value);

        /**
         * \brief   Binds a 64-bit integer value to the specified parameter index.
         * \param   index   The 0-based parameter index.
         * \param   value   The 64-bit integer value to bind.
         * \return  True if binding was successful, false otherwise.
         */
        bool bindInt64(int32_t index, int64_t value);
        bool bindUint64(int32_t index, uint64_t value);

        /**
         * \brief   Binds a double value to the specified parameter index.
         * \param   index   The 0-based parameter index.
         * \param   value   The double value to bind.
         * \return  True if binding was successful, false otherwise.
         */
        bool bindDouble(int32_t index, double value);
        bool bindFloat(int32_t index, float value);

        /**
         * \brief   Binds a text value to the specified parameter index.
         * \param   index   The 0-based parameter index.
         * \param   value   The string value to bind.
         * \return  True if binding was successful, false otherwise.
         */
        bool bindText(int32_t index, const areg::String& value);

        /**
         * \brief   Binds a NULL value to the specified parameter index.
         * \param   index   The 0-based parameter index.
         * \return  True if binding was successful, false otherwise.
         */
        bool bindNull(int32_t index);

        /**
         * \brief   Clears all parameter bindings for the prepared statement.
         */
        void clearBindings();

        /**
         * \brief   Retrieves the integer value of the specified column in the current row.
         * \param   index   The 0-based column index.
         * \return  The integer value of the column.
         */
        int32_t getInt32(int32_t index) const;
        uint32_t getUint32(int32_t index) const;

        /**
         * \brief   Retrieves the 64-bit integer value of the specified column in the current row.
         * \param   index  The 0-based column index.
         * \return  The 64-bit integer value of the column.
         */
        int64_t getInt64(int32_t index) const;
        uint64_t getUint64(int32_t index) const;

        /**
         * \brief   Retrieves the double value of the specified column in the current row.
         * \param   index  The 0-based column index.
         * \return  The double value of the column.
         */
        double getDouble(int32_t index) const;
        float getFloat(int32_t index) const;

        /**
         * \brief   Retrieves the text value of the specified column in the current row.
         * \param   index  The 0-based column index.
         * \return  The string value of the column.
         */
        areg::String getText(int32_t index) const;

        /**
         * \brief   Checks if the specified column in the current row is NULL.
         * \param   index  The 0-based column index.
         * \return  True if the column is NULL, false otherwise.
         */
        bool isNull(int32_t column) const;

        /**
         * \brief   Checks if the specified column index is valid for the current result set.
         * \param   index  The 0-based column index.
         * \return  True if the column index is valid, false otherwise.
         */
        bool isColumnValid(int32_t index) const;

        /**
         * \brief   Checks if the specified column contains a string value.
         * \param   index  The 0-based column index.
         * \return  True if the column is a string, false otherwise.
         */
        bool isString(int32_t index) const;

        /**
         * \brief   Checks if the specified column contains a 32-bit integer value.
         * \param   index  The 0-based column index.
         * \return  True if the column is a 32-bit integer, false otherwise.
         */
        bool isInteger(int32_t index) const;

        /**
         * \brief   Checks if the specified column contains a 64-bit integer value.
         * \param   index  The 0-based column index.
         * \return  True if the column is a 64-bit integer, false otherwise.
         */
        bool isInteger64(int32_t index) const;

        /**
         * \brief   Checks if the specified column contains a double value.
         * \param   index  The 0-based column index.
         * \return  True if the column is a double, false otherwise.
         */
        bool isDouble(int32_t index) const;

        /**
         * \brief   Returns the number of columns in the result set.
         * \return  The number of columns.
         */
        int32_t getColumnCount() const;

        /**
         * \brief   Returns the name of the specified column.
         * \param   index  The 0-based column index.
         * \return  The name of the column.
         */
        areg::String getColumnName(int32_t index) const;

        /**
         * \brief   Returns the index of the column with the specified name.
         * \param   columnName The name of the column.
         * \return  The 0-based index of the column, or -1 if not found.
         */
        int32_t getColumnIndex(const areg::String& columnName) const;

        /**
         * \brief   Returns the type of the specified column.
         * \param   index  The 0-based column index.
         * \return  The column type as ColumnType.
         */
        SqliteStatement::ColumnType getColumnType(int32_t index) const;

        /**
         * \brief   Returns a SqliteRow object representing the current row.
         * \return  The current SqliteRow.
         */
        aregext::SqliteRow row() const;

        /**
         * \brief   Advances to the next row and returns a SqliteRow object for it.
         * \return  The next SqliteRow.
         */
        aregext::SqliteRow nextRow() const;

        /**
         * \brief   Advances to the next row in the result set (const overload).
         * \return  True if a new row is available, false otherwise.
         */
        bool next() const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    protected:
        aregext::SqliteDatabase& mDatabase;  //!< The SQLite database object, which owns this statement.
        void *          mStatement; //!< The SQLite statement object.
        uint32_t        mRowPos;    //!< The current row position.

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
        SqliteStatement() = delete; //!< Default constructor is not allowed.
        AREG_NOCOPY_NOMOVE(SqliteStatement); //!< No copy or move allowed.
    };

    inline bool SqliteStatement::isValid() const
    {
        return (mStatement != nullptr);
    }


    inline uint32_t SqliteStatement::getRowPos() const
    {
        return mRowPos;
    }

} // namespace aregext
#endif // AREG_AREGEXTEND_DB_SQLITESTATEMENT_HPP
