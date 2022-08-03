# TableKM

A smart data manager that handles and helps automating large data sets.

## Table

A table can have any number of rows and columns. Columns will have strict data types which includes int32, int64, float32, float64, string, boolean, date and date_time.
All of these types takes a fixed amount of memory. The first column is the primary column, it can have duplicates. First column is always sorted either in
ascending or descending order.

Creating a table

![Alt creating a table](screenshots/creating_table.PNG?raw=true "Title")

After some insertions let us add two columns and fill it using **FORMULA**

![Alt adding column](screenshots/adding_new_column.png?raw=true "Title")

## Formula

We can add new column and generate value using a formula, refill existing column with formula, automate row insertions, create views, run queries and many more.
Formula are case sensitive, type restrictive and only supports functions. For example, to fill a column with  a+b or a-b depending on a > b, formula would be

```php
IF(isGreater($a,$b), add($a,$b), sub($a,$b))
```

There's a lot of functions available which you can find in help section.

![Alt formula](screenshots/formula.png?raw=true "Title")

Transforming a column with formula : in below example, formula is used to add 5% in percentage column

![Alt formula](screenshots/transforming_existing_column.PNG?raw=true "Title")

Automating row insertion : As we can assume "Total" is always "(mCS + mPhysics + mMath)" in our examples, so in next insertion, we would like to fill "Total"
automaticaly for that tapping on the "f" sign will provide this window

![Alt formula](screenshots/automate_adding_rows.png?raw=true "Title")

We can use formula for most of the things to automate our work. It is highly optimized and provides fast execution.

## Query

We can run queries to search specific data. Here's an example
Running sql like query to select student with CS Marks >= 75

![Alt running query](screenshots/running_query.png?raw=true "Title")

Then the query result

![Alt query result](screenshots/query_result.PNG?raw=true "Title")

## View

Views are just virtual tables which keeps track of the table and selects rows and column based on certain FILTER/condition formula.
It is auto refreshing and always up to date, doesn't consume memory to store those data instead it refers to the original table for fetching data.
Unlike table, it can be sorted with any column in any order. It is read only so user can not edit data through it.

Creating a view

![Alt create view](screenshots/createing_view.PNG?raw=true "Title")

The result

![Alt view result](screenshots/view_result.PNG?raw=true "Title")
