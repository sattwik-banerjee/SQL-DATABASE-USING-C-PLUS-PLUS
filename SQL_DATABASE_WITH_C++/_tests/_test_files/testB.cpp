#include "gtest/gtest.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>

//------------------------------------------------------------------------------------------
//Files we are testing:
#include "../../includes/sql/sql.h"
#include "../../includes/bplustree/multimap.h"

//------------------------------------------------------------------------------------------

using namespace std;

const vector<string> command_list = {

/*00*/     "make table employee fields last, first, dep, salary, year",
/*01*/     "insert into employee values Blow, Joe, CS, 100000, 2018",
/*02*/     "insert into employee values Blow, JoAnn, Physics, 200000, 2016",
/*03*/     "insert into employee values Johnson, Jack, HR, 150000, 2014",
/*04*/     "insert into employee values Johnson, \"Jimmy\", Chemistry, 140000, 2018",
/*05*/     "insert into employee values Johnson, \"Billy Bob\", Physics, 142000, 2014",
/*06*/     "insert into employee values Smith, John, Math, 120000, 2015",
/*07*/     "insert into employee values Smith, Jane, CS, 110000, 2017",
/*08*/     "insert into employee values Patel, Amit, Physics, 135000, 2016",
/*09*/     "insert into employee values Patel, Priya, HR, 125000, 2019",
/*10*/     "insert into employee values Kim, Sun, Chemistry, 130000, 2017",
/*11*/     "insert into employee values Kim, Soo, CS, 115000, 2018",
/*12*/     "insert into employee values Lee, Min, Math, 125000, 2019",
/*13*/     "insert into employee values Lee, \"Soo-Jin\", Physics, 140000, 2016",
/*14*/     "insert into employee values Brown, Mike, CS, 110000, 2017",
/*15*/     "insert into employee values Brown, Sarah, HR, 135000, 2015",
/*16*/     "insert into employee values White, James, Chemistry, 145000, 2014",
/*17*/     "insert into employee values White, Maria, Math, 150000, 2016",
/*18*/     "insert into employee values Davis, Alex, Physics, 155000, 2018",
/*19*/     "insert into employee values Davis, Anna, CS, 160000, 2019",
/*20*/     "insert into employee values Garcia, Carlos, HR, 105000, 2017",
/*21*/     "insert into employee values Garcia, \"Luis Miguel\", Chemistry, 110000, 2016",
/*22*/     "insert into employee values Martinez, Laura, Math, 115000, 2015",
/*23*/     "insert into employee values Martinez, Juan, Physics, 120000, 2014",
/*24*/     "insert into employee values Hernandez, Diego, CS, 125000, 2019",
/*25*/     "insert into employee values Hernandez, Sofia, HR, 130000, 2018",
/*26*/     "insert into employee values Robinson, Emma, Chemistry, 135000, 2017",
/*27*/     "insert into employee values Robinson, Jack, Math, 140000, 2016",
/*28*/     "insert into employee values Clark, Olivia, Physics, 145000, 2015",
/*29*/     "insert into employee values Clark, Ethan, CS, 150000, 2014",
/*30*/     "insert into employee values Rodriguez, Ava, HR, 155000, 2019",
/*31*/     "insert into employee values Rodriguez, Lucas, Chemistry, 160000, 2018",
/*32*/     "insert into employee values Lewis, Mia, Math, 165000, 2017",
/*33*/     "insert into employee values Lewis, Oliver, Physics, 170000, 2016",
/*34*/     "insert into employee values Walker, Amelia, CS, 175000, 2015",
/*35*/     "insert into employee values Walker, Mason, HR, 180000, 2014",
/*36*/     "insert into employee values Hall, Harper, Chemistry, 185000, 2019",
/*37*/     "insert into employee values Hall, Logan, Math, 190000, 2018",
/*38*/     "insert into employee values Allen, Ella, Physics, 195000, 2017",
/*39*/     "insert into employee values Allen, Jacob, CS, 200000, 2016",
/*40*/     "insert into employee values Young, Scarlett, HR, 205000, 2015",
/*41*/     "insert into employee values Young, Jameson, Chemistry, 210000, 2014",
/*42*/     "insert into employee values King, Avery, Math, 215000, 2019",
/*43*/     "insert into employee values King, Benjamin, Physics, 220000, 2018",
/*44*/     "insert into employee values Wright, Aria, CS, 225000, 2017",
/*45*/     "insert into employee values Wright, Henry, HR, 230000, 2016",
/*46*/     "insert into employee values Scott, Mila, Chemistry, 235000, 2015",
/*47*/     "insert into employee values Scott, Daniel, Math, 240000, 2014",
/*48*/     "insert into employee values Green, Layla, Physics, 245000, 2019",
/*49*/     "insert into employee values Green, Matthew, CS, 250000, 2018",
/*50*/     "insert into employee values Adams, Chloe, HR, 255000, 2017",

/*15*/    "select * from employee where dep = CS",            // Select employees with department CS
/*16*/    "select * from employee where salary < 125000",     // Select employees with salary less than 125000
/*17*/    "select * from employee where dep = Physics",       // Select employees with department Physics
/*18*/    "select * from employee where salary >= 130000",    // Select employees with salary greater than or equal to 130000
/*19*/    "select * from employee where year > 2017",         // Select employees hired after 2017
/*20*/    "select * from employee where last = Johnson",      // Select employees with last name Johnson
/*21*/    "select * from employee where first = Amit",        // Select employees with first name Amit
/*22*/    "select * from employee where salary = 140000",     // Select employees with salary equal to 140000
/*23*/    "select * from employee where first = Soo",           // Select employees not in department CS
/*24*/    "select * from employee where year <= 2016",         // Select employees hired on or before 2016

/*25*/    "select last from employee where salary <= 120000",        // Select employees with salary less than or equal to 120000
/*26*/    "select first, dep, salary from employee where dep = HR and year >= 2018", // Select HR department employees hired in 2018 or later
/*27*/    "select first, year, dep from employee where dep = Chemistry and first >= Kim", // Select Chemistry department employees with first name >= Kim
/*28*/    "select last, first, dep from employee where salary > 110000 and salary < 140000", // Select employees with salary between 110000 and 140000
/*29*/    "select dep, salary, last from employee where dep = Physics or year < 2016", // Select Physics department employees hired in years other than 2016

/*30*/    "select * from employee where (dep = lol or dep = Physics)",    // Select employees from CS or Physics department
/*31*/    "select * from employee where last > 'J'",                    // Select employees with last name greater than J
/*32*/    "select * from employee where last > 'J' and (dep = CS or dep = Physics)", // Select employees with last name greater than J in CS or Physics department
/*33*/    "select first, dep from employee where salary >= 150000",     // Select first names and departments of employees with salary >= 150000
/*34*/    "select last, year from employee where year <= 2017",         // Select last names and hire years of employees hired on or before 2017
/*35*/    "select dep, salary from employee where dep = HR and salary > 130000", // Select HR employees with salary greater than 130000
/*36*/    "select last, first from employee where dep = Chemistry",     // Select last and first names of employees in Chemistry department
/*37*/    "select last, first from employee where year < 2016",         // Select last and first names of employees hired before 2016
/*38*/    "select first, salary from employee where salary <= 115000",  // Select first names and salaries of employees with salary <= 115000
/*39*/    "select dep, year from employee where year > 2015",           // Select departments and hire years of employees hired after 2015
/*40*/    "select first, last from employee where dep = Math and salary > 120000", // Select first and last names of Math employees with salary > 120000
/*41*/    "select * from employee where first = \"Soo-Jin\"",             // Select all details of employees with first name Soo-Jin
/*42*/    "select last, dep from employee where dep = CS or salary >= 130000", // Select last names and departments of employees in CS or with salary >= 130000
/*43*/    "select dep, first, year from employee where year < 2018",    // Select departments, first names, and hire years of employees hired before 2018
/*44*/    "select last, first from employee where dep = Physics and salary >= 140000", // Select last and first names of Physics employees with salary >= 140000
/*45*/    "select * from employee where year = 2016",                   // Select all details of employees hired in 2016
/*46*/    "select last, first from employee where last = 'Lee'",        // Select last and first names of employees with last name Lee
/*47*/    "select dep, salary, year from employee where dep = Chemistry and year > 2015", // Select departments, salaries, and hire years of Chemistry employees hired after 2015
/*48*/    "select first, last, dep from employee where salary <= 110000", // Select first names, last names, and departments of employees with salary <= 110000
/*49*/    "select * from employee where (dep = HR or dep = Math) and year >= 2015", // Select all details of HR and Math employees hired in or after 2015
/*50*/    "select * from employee where dep = CS",                   // Select employees with department CS
/*51*/    "select * from employee where salary > 150000",            // Select employees with salary greater than 150000
/*52*/    "select * from employee where dep = Chemistry",            // Select employees with department Chemistry
/*53*/    "select * from employee where year < 2014",                // Select employees hired before 2014
/*54*/    "select * from employee where first = John",               // Select employees with first name John
/*55*/    "select * from employee where salary >= 125000",           // Select employees with salary not equal to 125000
/*56*/    "select * from employee where last = Smith",               // Select employees with last name Smith
/*57*/    "select * from employee where dep = HR",                   // Select employees with department HR
/*58*/    "select * from employee where year >= 2019",               // Select employees hired in 2019 or later
/*59*/    "select * from employee where salary <= 130000",           // Select employees with salary less than or equal to 130000
/*60*/    "select * from employee where first = Jane",               // Select employees with first name Jane
/*61*/    "select last, first from employee where salary > 115000",  // Select last and first names of employees with salary greater than 115000
/*62*/    "select dep, year from employee where dep = CS",           // Select departments and hire years of employees in CS department
/*63*/    "select * from employee where last < M",                   // Select employees with last name less than M
/*64*/    "select first, last from employee where dep = Math",       // Select first and last names of employees in Math department
/*65*/    "select dep, salary from employee where year < 2017",      // Select departments and salaries of employees hired before 2017
/*66*/    "select last, first from employee where year = 2015",      // Select last and first names of employees hired in 2015
/*67*/    "select dep, year from employee where dep = Physics",      // Select departments and hire years of employees in Physics department
/*68*/    "select * from employee where salary > 140000",            // Select employees with salary greater than 140000
/*69*/    "select * from employee where first = Mike",               // Select employees with first name Mike
/*70*/    "select dep, salary from employee where dep = Chemistry",  // Select departments and salaries of employees in Chemistry department
/*71*/    "select * from employee where last = Patel",               // Select employees with last name Patel
/*72*/    "select last, first from employee where dep = HR and salary < 120000",  // Select last and first names of HR employees with salary less than 120000
/*73*/    "select dep, year from employee where year >= 2016",       // Select departments and hire years of employees hired in or after 2016
/*74*/    "select * from employee where salary <= 100000",           // Select employees with salary less than or equal to 100000
/*75*/    "select * from employee where first = Priya",              // Select employees with first name Priya
/*76*/    "select last, dep from employee where dep = Math and year > 2014",  // Select last names and departments of Math employees hired after 2014
/*77*/    "select dep, salary from employee where dep = Physics and salary > 130000", // Select departments and salaries of Physics employees with salary greater than 130000
/*78*/    "select * from employee where last < C",                   // Select employees with last name less than C
/*79*/    "select first, last from employee where salary > 125000",  // Select first and last names of employees with salary greater than 125000
/*80*/    "select dep, year from employee where dep = CS and year < 2017", // Select departments and hire years of CS employees hired before 2017

// /*00*/     "make table student fields id, name, major, gpa, graduation_year",
// /*01*/     "make table product fields product_id, name, category, price, stock_quantity",
// /*02*/     "make table book fields isbn, title, author, publisher, year_published",
// /*03*/     "make table movie fields movie_id, title, director, genre, release_year",
// /*04*/     "make table customer fields customer_id, first_name, last_name, email, phone",
// /*05*/     "make table order fields order_id, customer_id, product_id, quantity, order_date",
// /*06*/     "make table company fields company_id, name, industry, revenue, number_of_employees",
// /*07*/     "make table library fields library_id, name, location, books_count, members_count",
// /*08*/     "make table flight fields flight_id, airline, departure_city, arrival_city, departure_time",
// /*09*/     "make table department fields dept_id, name, manager, budget, location",
// /*10*/     "make table vehicle fields vin, model, year, price",
// /*11*/     "make table hospital fields hospital_id, name, location, capacity, number_of_staff",
// /*12*/     "make table event fields event_id, name, date, location, organizer",
// /*13*/     "make table musician fields musician_id, first_name, last_name, instrument, genre",
// /*14*/     "make table restaurant fields restaurant_id, name, cuisine, location, rating",
// /*15*/     "make table course fields course_id, title, instructor, credits, department",
// /*16*/     "make table tournament fields tournament_id, name, sport, location, prize_money",
// /*17*/     "make table artwork fields artwork_id, title, artist, year_created, medium",
// /*18*/     "make table planet fields planet_id, name, distance_from_sun, radius, number_of_moons",
// /*19*/     "make table zoo fields zoo_id, name, location, number_of_animals, annual_visitors",
// /*20*/     "make table musician fields musician_id, first_name, last_name, instrument, genre",
// /*21*/     "make table movie fields movie_id, title, director, genre, release_year",
// /*22*/     "make table city fields city_id, name, country, population, area",
// /*23*/     "make table album fields album_id, title, artist, release_date, genre",
// /*24*/     "make table teacher fields teacher_id, first_name, last_name, subject, hire_date",
// /*25*/     "make table athlete fields athlete_id, first_name, last_name, sport, medals",
// /*26*/     "make table museum fields museum_id, name, location, founded_year, number_of_artworks",
// /*27*/     "make table author fields author_id, first_name, last_name, nationality, birth_date",
// /*28*/     "make table park fields park_id, name, location, area, established_year",
// /*29*/     "make table song fields song_id, title, artist, album, duration",
// /*30*/     "make table match fields match_id, team1, team2, date, location",
// /*31*/     "make table magazine fields magazine_id, title, publisher, frequency, circulation",
// /*32*/     "make table hotel fields hotel_id, name, location, number_of_rooms, star_rating",
// /*33*/     "make table festival fields festival_id, name, location, start_date, end_date",
// /*34*/     "make table scientist fields scientist_id, first_name, last_name, field, notable_work",
// /*35*/     "make table spacecraft fields spacecraft_id, name, mission_type, launch_date, status",
// /*36*/     "make table winery fields winery_id, name, location, established_year, number_of_wines",
// /*37*/     "make table gadget fields gadget_id, name, brand, price, release_year",
// /*38*/     "make table project fields project_id, name, leader, start_date, end_date",
// /*39*/     "make table volcano fields volcano_id, name, country, last_eruption_year, elevation",
// /*00*/    "create table student fields id, first_name, last_name, major, gpa",
// /*01*/    "create table course fields course_id, course_name, credits, department",
// /*02*/    "create table enrollment fields student_id, course_id, grade",
// /*03*/    "create table professor fields prof_id, first_name, last_name, department, salary",
// /*04*/    "create table department fields dep_id, dep_name, building, budget",
// /*05*/    "create table library fields lib_id, lib_name, location, num_books",
// /*06*/    "create table book fields book_id, title, author, pub_year, genre",
// /*07*/    "create table loan fields loan_id, book_id, student_id, due_date",
// /*08*/    "create table publisher fields pub_id, pub_name, address, phone",
// /*09*/    "create table author fields author_id, first_name, last_name, birthdate",
// /*10*/    "create table event fields event_id, event_name, event_date, location, host",
// /*11*/    "create table ticket fields ticket_id, event_id, price, seat_number",
// /*12*/    "create table vendor fields vendor_id, vendor_name, contact_name, contact_phone",
// /*13*/    "create table sale fields sale_id, product_id, quantity, sale_date",
// /*14*/    "create table product fields product_id, product_name, price, stock",
// /*15*/    "create table inventory fields inventory_id, product_id, quantity, location",
// /*16*/    "create table order fields order_id, customer_id, order_date, total_amount",
// /*17*/    "create table customer fields customer_id, first_name, last_name, email, phone",
// /*18*/    "create table shipment fields shipment_id, order_id, ship_date, delivery_date",
// /*19*/    "create table payment fields payment_id, order_id, payment_date, amount, method",
// /*20*/    "create table supplier fields supplier_id, supplier_name, contact_name, contact_phone"

};

const int MAKE_TABLE_COMMANDS = 51;
const int SELECT_COMMANDS = 115;

bool sql_basic(bool debug = false)
{
     SQL sql;
     Table t;
     cout << ">" << command_list[0] << endl;
     cout << sql.command(command_list[0]);
     cout << "basic_test: table created." << endl<<endl;

     for (int i = 0; i < MAKE_TABLE_COMMANDS; i++)
     {
          cout << ">" << command_list[i] << endl;
          // cout << 
          cout << sql.command(command_list[i]);
     }

     cout << sql.get_table();

     cout << endl
          << endl;
     for (int i = MAKE_TABLE_COMMANDS; i < command_list.size(); i++)
     {
          cout << "\n>" << command_list[i] << endl;
          if (debug)
               cout<< sql.command(command_list[i])<<endl;
          else
               t = sql.command(command_list[i]);
          cout << "basic_test: records selected: "<<sql.select_recnos() << endl;
     }

     cout << "----- END TEST --------" << endl;
     return true;
}

bool mmap_vec(bool debug = false)
{
    std::string firstNames[10] = {
        "Alice", "Bob", "Charlie", "David", "Eve",
        "Frank", "Grace", "Hannah", "Ivy", "Jack"
    };

    // Array of last names
    std::string lastNames[10] = {
        "Smith", "Johnson", "Williams", "Brown", "Jones",
        "Garcia", "Miller", "Davis", "Martinez", "Rodriguez"
    };

    // Array of ages (15-25)
    std::string ages[10] = {
        "15", "16", "17", "18", "19",
        "20", "21", "22", "23", "24"
    };

    // Array of departments
    std::string departments[10] = {
        "CS", "Art", "Biology", "Chemistry", "Physics",
        "Mathematics", "History", "Literature", "Economics", "Engineering"
    };

    // Array of cities
    std::string cities[10] = {
        "New York", "Los Angeles", "Chicago", "Houston", "Phoenix",
        "Philadelphia", "San Antonio", "San Diego", "Dallas", "San Jose"
    };

    vectorstr fields={"fname", "lname", "age", "dept", "city"};

    Table t("student", fields);
    vectorstr row;
    for(int i = 0; i < 10; i++)
    {
       row = {firstNames[i], lastNames[i], ages[i], departments[i], cities[i]};
       t.insert_into(row);
    }
    for(int i = 0; i < 10000; i++)
    {
        Table temp("student");
    }
    return true;
}

// ==============================
// global BAD!
bool debug = false;
// ==============================

// TEST(SQL_BASIC, SQLBasic) {
//   bool success = sql_basic(debug);
//   EXPECT_EQ(success, true);
// }

TEST(MMAP_TEST, TestMMap)
{
  bool success = mmap_vec(debug);
  EXPECT_EQ(success, true);
}

int main(int argc, char **argv) {
  if (argc > 1)
  {
    debug = !strcmp(argv[1], "debug");
  }

  ::testing::InitGoogleTest(&argc, argv);
  std::cout<<"\n\n----------running basic_test.cpp---------\n\n"<<std::endl;
  return RUN_ALL_TESTS();
}
/*
build git:(master) ✗  😊 $> tree ../includes
../includes
├── binary_files
│   ├── file_record.cpp
│   ├── file_record.h
│   ├── utilities.cpp
│   └── utilities.h
├── bplustree
│   ├── bplustree.h
│   ├── btree_array_funcs.h
│   ├── map.h
│   ├── multimap.h
│   └── set_class.h
├── linked_list_functions
│   └── linkedlistfunctionsTemplated.h
├── node
│   ├── bogus.txt
│   ├── master.txt
│   └── node.h
├── parser
│   ├── parser.cpp
│   ├── parser.h
│   ├── sql_parser_functions.cpp
│   ├── sql_parser_functions.h
│   └── typedefs.h
├── queue
│   ├── MyQueue.h
│   └── queue_composition.h
├── shunting_yard
│   ├── shunting_yard.cpp
│   └── shunting_yard.h
├── sql
│   ├── sql.cpp
│   └── sql.h
├── stack
│   ├── MyStack.h
│   └── stack_composition.h
├── stl_utils
│   └── vector_utilities.h
├── table
│   ├── table.cpp
│   ├── table.h
│   └── typedefs.h
├── token
│   ├── child_tokens.h
│   ├── token.cpp
│   └── token.h
└── tokenizer
    ├── constants.h
    ├── ftokenize.cpp
    ├── ftokenize.h
    ├── state_machine_functions.cpp
    ├── state_machine_functions.h
    ├── stokenize.cpp
    └── stokenize.h


//==========================================================================
//  debug = off
//==========================================================================


build git:(master) ✗  😊 $> ./bin/basic_test       


----------running basic_test.cpp---------


[==========] Running 1 test from 1 test case.
[----------] Global test environment set-up.
[----------] 1 test from SQL_BASIC
[ RUN      ] SQL_BASIC.SQLBasic


------ SQL Tables I Manage: --------
employee
student
---------------------------


>make table employee fields  last,       first,         dep,      salary, year
create table basic_test: table created.

>make table employee fields  last,       first,         dep,      salary, year
create table >insert into employee values Blow,       Joe,           CS,       100000, 2018
>insert into employee values Blow,       JoAnn,         Physics,  200000, 2016
>insert into employee values Johnson,    Jack,          HR,       150000, 2014
>insert into employee values Johnson,    "Jimmy",     Chemistry,140000, 2018
>make table student fields  fname,          lname,    major,    age
create table >insert into student values Flo,            Yao,   Art,    20
>insert into student values Bo,                      Yang,      CS,             28
>insert into student values "Sammuel L.", Jackson,      CS,             40
>insert into student values "Billy",         Jackson,   Math,   27
>insert into student values "Mary Ann",   Davis,        Math,   30



>select * from employee
There is no condition
basic_test: records selected: 0 1 2 3 

>select last, first, age from employee
There is no condition
basic_test: records selected: 0 1 2 3 

>select last from employee
There is no condition
basic_test: records selected: 0 1 2 3 

>select * from employee where last = Johnson
basic_test: records selected: 2 3 

>select * from employee where last=Blow and major="JoAnn"
basic_test: records selected: 

>select * from student
There is no condition
basic_test: records selected: 0 1 2 3 4 

>select * from student where (major=CS or major=Art)
basic_test: records selected: 0 1 2 

>select * from student where lname>J
basic_test: records selected: 2 3 1 0 

>select * from student where lname>J and (major=CS or major=Art)
basic_test: records selected: 0 1 2 
----- END TEST --------
[       OK ] SQL_BASIC.SQLBasic (135 ms)
[----------] 1 test from SQL_BASIC (135 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test case ran. (137 ms total)
[  PASSED  ] 1 test.
build git:(master) ✗  😊 $> 






//==========================================================================
//  debug = on
//==========================================================================

build git:(master) ✗  😊 $> ./bin/basic_test debug


----------running basic_test.cpp---------


[==========] Running 1 test from 1 test case.
[----------] Global test environment set-up.
[----------] 1 test from SQL_BASIC
[ RUN      ] SQL_BASIC.SQLBasic


------ SQL Tables I Manage: --------
employee
student
---------------------------


>make table employee fields  last,       first,         dep,      salary, year
create table basic_test: table created.

>make table employee fields  last,       first,         dep,      salary, year
create table >insert into employee values Blow,       Joe,           CS,       100000, 2018
>insert into employee values Blow,       JoAnn,         Physics,  200000, 2016
>insert into employee values Johnson,    Jack,          HR,       150000, 2014
>insert into employee values Johnson,    "Jimmy",     Chemistry,140000, 2018
>make table student fields  fname,          lname,    major,    age
create table >insert into student values Flo,            Yao,   Art,    20
>insert into student values Bo,                      Yang,      CS,             28
>insert into student values "Sammuel L.", Jackson,      CS,             40
>insert into student values "Billy",         Jackson,   Math,   27
>insert into student values "Mary Ann",   Davis,        Math,   30



>select * from employee
There is no condition

Table name: _select_table_34, records: 4
                   record                     last                    first                      dep                   salary                     year

                        0                     Blow                      Joe                       CS                   100000                     2018
                        1                     Blow                    JoAnn                  Physics                   200000                     2016
                        2                  Johnson                     Jack                       HR                   150000                     2014
                        3                  Johnson                    Jimmy                Chemistry                   140000                     2018


basic_test: records selected: 0 1 2 3 

>select last, first, age from employee
There is no condition

Table name: _select_table_38, records: 4
                   record                     last                    first                      age

                        0                     Blow                      Joe                     Blow
                        1                     Blow                    JoAnn                     Blow
                        2                  Johnson                     Jack                  Johnson
                        3                  Johnson                    Jimmy                  Johnson


basic_test: records selected: 0 1 2 3 

>select last from employee
There is no condition

Table name: _select_table_42, records: 4
                   record                     last

                        0                     Blow
                        1                     Blow
                        2                  Johnson
                        3                  Johnson


basic_test: records selected: 0 1 2 3 

>select * from employee where last = Johnson

Table name: _select_table_46, records: 2
                   record                     last                    first                      dep                   salary                     year

                        0                  Johnson                     Jack                       HR                   150000                     2014
                        1                  Johnson                    Jimmy                Chemistry                   140000                     2018


basic_test: records selected: 2 3 

>select * from employee where last=Blow and major="JoAnn"

Table name: _select_table_50, records: 0
                   record                     last                    first                      dep                   salary                     year



basic_test: records selected: 

>select * from student
There is no condition

Table name: _select_table_54, records: 5
                   record                    fname                    lname                    major                      age

                        0                      Flo                      Yao                      Art                       20
                        1                       Bo                     Yang                       CS                       28
                        2              Sammuel L .                  Jackson                       CS                       40
                        3                    Billy                  Jackson                     Math                       27
                        4                 Mary Ann                    Davis                     Math                       30


basic_test: records selected: 0 1 2 3 4 

>select * from student where (major=CS or major=Art)

Table name: _select_table_58, records: 3
                   record                    fname                    lname                    major                      age

                        0                      Flo                      Yao                      Art                       20
                        1                       Bo                     Yang                       CS                       28
                        2              Sammuel L .                  Jackson                       CS                       40


basic_test: records selected: 0 1 2 

>select * from student where lname>J

Table name: _select_table_62, records: 4
                   record                    fname                    lname                    major                      age

                        0              Sammuel L .                  Jackson                       CS                       40
                        1                    Billy                  Jackson                     Math                       27
                        2                       Bo                     Yang                       CS                       28
                        3                      Flo                      Yao                      Art                       20


basic_test: records selected: 2 3 1 0 

>select * from student where lname>J and (major=CS or major=Art)

Table name: _select_table_66, records: 3
                   record                    fname                    lname                    major                      age

                        0                      Flo                      Yao                      Art                       20
                        1                       Bo                     Yang                       CS                       28
                        2              Sammuel L .                  Jackson                       CS                       40


basic_test: records selected: 0 1 2 
----- END TEST --------
[       OK ] SQL_BASIC.SQLBasic (84 ms)
[----------] 1 test from SQL_BASIC (84 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test case ran. (84 ms total)
[  PASSED  ] 1 test.
build git:(master) ✗  😊 $> 







*/