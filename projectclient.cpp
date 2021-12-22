#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Input Methods
int get_int_input()
{
  int input;
  cin >> input;
  return input;
}

string get_str_input()
{
  string input = "";
  cin >> input;
  return input;
}

// Query functions
int query(MYSQL *pMysql, string queryBuffer, bool id_counting)
{
  MYSQL_RES *pQueryResult; // Pointer to query result handler
  MYSQL_ROW row;           // Space for result row
  int numFields;

  // Convert string to char array
  int query_length = queryBuffer.length();
  char query[query_length + 1];
  strcpy(query, queryBuffer.c_str());

  // Run the query
  int rc = mysql_query(pMysql, query);
  if (rc)
  {
    cerr << ": mysql_query() error: " << mysql_error(pMysql) << endl
         << "rc: " << rc << endl;
    cerr << "The query ran was: " << query << endl;
    return -1;
  }

  // Fetch the results
  pQueryResult = mysql_store_result(pMysql);
  numFields = mysql_field_count(pMysql); // And get the field count
  if (!pQueryResult)
  { // We got nothing back; that may be OK
    // cout << "!pQueryResult" << endl;
    if (numFields == 0)
    { // We should have nothing back!
      cerr << "SQL Information: Query \"" << queryBuffer << "\" returned zero rows" << endl
           << "ADD / UPDATE success" << endl;
    }
    else
    { // somehow a SELECT has an empty result
      cerr << "SQL Error: Query \"" << queryBuffer << "\" failed to return expected data" << endl
           << "SQL error information: " << mysql_error(pMysql) << endl;
      return -1;
    }
  }
  else
  { // Retrieve the rows of SELECT query
    if (id_counting)
    {
      row = mysql_fetch_row(pQueryResult);
      int id = atoi(row[0]);
      return id;
    }

    cout << "Database";
    cout << "--------------" << endl;
    while ((row = mysql_fetch_row(pQueryResult)))
    {
      unsigned long *lengths;
      lengths = mysql_fetch_lengths(pQueryResult);
      for (int i = 0; i < numFields; i++)
        printf("%.*s ", (int)lengths[i], row[i] ? row[i] : "NULL");
      printf("\n");
    }
    cout << "--------------" << endl;
  }
  mysql_free_result(pQueryResult);

  return 0;

  // mysql_close(pMysql);
}

// Functional Methods
int add_entry(MYSQL *pMysql)
{
  /*
  Add Menu:
    1. Which database to add to. (london_outcomes, stop_and_search, crime_by_LSOA)
    2. Ask for all fields 1 by 1. (Verify that fields cant be NULL, error handling)
  */

  cout << "Select the database to add a crime to." << endl;
  cout << "Enter 1 to add to london_outcomes." << endl;
  cout << "Enter 2 to add to stop_and_search." << endl;
  cout << "Enter 3 to add to london_crime_by_LSOA." << endl;
  cout << "Enter 0 to exit client." << endl;
  int int_input = get_int_input();

  string table_name = "";
  string add_prefix = "INSERT INTO ";

  /*
  INSERT INTO table_name (column1, column2, column3, ...)
  VALUES (value1, value2, value3, ...);
  */

  if (int_input == 1)
  {
    // Add to london_outcomes
    // 1. Add to london_crime.

    // Add to london_crime
    table_name = "london_crime ";
    string str_input = "";

    vector<string> lcrime_attributes{"crime_id", "month", "reported_by", "falls_within", "latitude", "longitude", "location", "lsoa_code"};
    vector<string> lcrime_attr_values;

    cin.ignore();

    for (int i = 0; i < lcrime_attributes.size(); i++)
    {
      cout << "Enter " + lcrime_attributes[i] + ":" << endl;
      //      cin >> str_input;
      getline(cin, str_input);

      lcrime_attr_values.push_back(str_input);
    }

    // Construct Add query given attribute values.
    string lcrime_query = add_prefix + table_name + "(" + "crime_ID, month, reported_by, falls_within, latitude, longitude, location, lsoa_code" + ") " + "VALUES " + "(" + "'" + lcrime_attr_values[0] + "'" + "," + "'" + lcrime_attr_values[1] + "'" + "," + "'" + lcrime_attr_values[2] + "'" + "," + "'" + lcrime_attr_values[3] + "'" + "," + "'" + lcrime_attr_values[4] + "'" + "," + "'" + lcrime_attr_values[5] + "'" + "," + "'" + lcrime_attr_values[6] + "'" + "," + "'" + lcrime_attr_values[7] + "'" + ")" + ";";

    // Debug output
    cout << lcrime_query << endl;

    // Run query
    query(pMysql, lcrime_query, false);

    // Get Auto-incremented offense_ID.
    int offense_ID = query(pMysql, "SELECT MAX(offense_ID) FROM london_crime", true);

    // Add to london_outcomes
    table_name = "london_outcomes ";
    str_input = "";

    vector<string> loutcomes_attributes{"outcome_type"};
    vector<string> loutcomes_attr_values;

    //    cin.ignore();

    for (int i = 0; i < loutcomes_attributes.size(); i++)
    {
      cout << "Enter " + loutcomes_attributes[i] + ":" << endl;
      //      cin >> str_input;
      getline(cin, str_input);

      loutcomes_attr_values.push_back(str_input);
    }

    // Construct Add query given attribute values.
    string loutcomes_query = add_prefix + table_name + "(" + "offense_ID, outcome_type" + ") " + "VALUES " + "(" + "'" + to_string(offense_ID) + "'" + "," + "'" + loutcomes_attr_values[0] + "'" + ")" + ";";

    // Debug output
    cout << loutcomes_query << endl;

    // Run query
    query(pMysql, loutcomes_query, false);
  }
  else if (int_input == 2)
  {
    // Add to stop_and_search
    // 1. Add to person, setting, investigation tables.
    // 2. Take PKs of those tables and add to stop_and_search ???

    // Add to setting.
    table_name = "setting ";
    vector<string> setting_attributes{"date", "latitude", "longitude"};
    vector<string> setting_attr_values;

    string str_input = "";

    cin.ignore();

    for (int i = 0; i < setting_attributes.size(); i++)
    {
      cout << "Enter " + setting_attributes[i] + ":" << endl;
      //      cin >> str_input;
      getline(cin, str_input);

      setting_attr_values.push_back(str_input);
    }

    string setting_query = add_prefix + table_name + "(" + "date, latitude, longitude" + ") " + "VALUES " + "(" + "'" + setting_attr_values[0] + "'" + "," + "'" + setting_attr_values[1] + "'" + "," + "'" + setting_attr_values[2] + "'" + ")" + ";";

    // Debug output
    cout << setting_query << endl;

    // Run query
    query(pMysql, setting_query, false);

    // NOW GET THE setting_ID, to be used to insert same one into all other tables
    int id_value = query(pMysql, "SELECT MAX(setting_ID) FROM setting", true);

    // Add to person.
    table_name = "person ";
    vector<string> person_attributes{"gender", "age_range", "self_defined_ethnicity", "officer_defined_ethnicity"};
    vector<string> person_attr_values;

    str_input = "";

    for (int i = 0; i < person_attributes.size(); i++)
    {
      cout << "Enter " + person_attributes[i] + ":" << endl;
      //      cin >> str_input;
      getline(cin, str_input);

      person_attr_values.push_back(str_input);
    }

    string person_query = add_prefix + table_name + "(" + "person_ID, gender, age_range, self_defined_ethnicity, officer_defined_ethnicity" + ") " + "VALUES " + "(" + "'" + to_string(id_value) + "'" + "," + "'" + person_attr_values[0] + "'" + "," + "'" + person_attr_values[1] + "'" + "," + "'" + person_attr_values[2] + "'" + "," + "'" + person_attr_values[3] + "'" + ")" + ";";

    // Debug output
    cout << person_query << endl;

    // Run query
    query(pMysql, person_query, false);

    // Add to investigation.
    table_name = "investigation ";
    vector<string> inv_attributes{"part_of_a_policing_operation", "legislation"};
    vector<string> inv_attr_values;

    str_input = "";

    for (int i = 0; i < inv_attributes.size(); i++)
    {
      cout << "Enter " + inv_attributes[i] + ":" << endl;
      //      cin >> str_input;
      getline(cin, str_input);

      inv_attr_values.push_back(str_input);
    }

    string investigation_query = add_prefix + table_name + "(" + "investigation_ID, part_of_a_policing_operation, legislation" + ") " + "VALUES " + "(" + "'" + to_string(id_value) + "'" + "," + "'" + inv_attr_values[0] + "'" + "," + "'" + inv_attr_values[1] + "'" + ")" + ";";

    // Debug output
    cout << investigation_query << endl;

    // Run query
    query(pMysql, investigation_query, false);

    // Add to stop_and_search
    table_name = "stop_and_search";
    vector<string> sas_attributes{"type", "object_of_search", "outcome"};
    vector<string> sas_attr_values;

    str_input = "";

    /*
    setting_ID, person_ID, investigation_ID, date, latitude, longitude, 
    gender, age_range, self_defined_ethnicity, officer_defined_ethnicity,
    part_of_a_policing_operation, legislation, 
    */

    cout << "Type 'Exit' anytime to exit." << endl;

    for (int i = 0; i < sas_attributes.size(); i++)
    {
      cout << "Enter " + sas_attributes[i] + ":" << endl;
      //      cin >> str_input;
      getline(cin, str_input);

      sas_attr_values.push_back(str_input);
    }

    // Construct Add query given attribute values.
    string sas_query = add_prefix + table_name + "(" + "setting_ID, person_ID, investigation_ID, date, latitude, longitude, gender, age_range, self_defined_ethnicity, officer_defined_ethnicity, part_of_a_policing_operation, legislation, type, object_of_search, outcome" + ") " + "VALUES " + "(" + "'" + to_string(id_value) + "'" + "," + "'" + to_string(id_value) + "'" + "," + "'" + to_string(id_value) + "'" + "," + "'" + setting_attr_values[0] + "'" + "," + "'" + setting_attr_values[1] + "'" + "," + "'" + setting_attr_values[2] + "'" + "," + "'" + person_attr_values[0] + "'" + "," + "'" + person_attr_values[1] + "'" + "," + "'" + person_attr_values[2] + "'" + "," + "'" + person_attr_values[3] + "'" + "," + "'" + inv_attr_values[0] + "'" + "," + "'" + inv_attr_values[1] + "'" + "," + "'" + sas_attr_values[0] + "'" + "," + "'" + sas_attr_values[1] + "'" + "," + "'" + sas_attr_values[2] + "'" + ")" + ";";

    // Debug output
    cout << sas_query << endl;

    // Run query
    query(pMysql, sas_query, false);
  }
  else if (int_input == 3)
  {

    // Add to london_crime_by_lsoa

    table_name = "london_crime_by_lsoa";

    string str_input = "";

    vector<string> by_lsoa_attributes{"lsoa_code", "borough", "major_category", "minor_category", "value", "year", "month"};

    vector<string> attr_values;

    cin.ignore();

    for (int i = 0; i < by_lsoa_attributes.size(); i++)
    {
      cout << "Enter " + by_lsoa_attributes[i] + ":" << endl;
      //      cin >> str_input;
      getline(cin, str_input);

      attr_values.push_back(str_input);
    }

    // Construct Add query given attribute values.
    string by_lsoa_query = add_prefix + table_name + "(" + "lsoa_code, borough, major_category, minor_category, value, year, month" + ") " + "VALUES " + "(" + "'" + attr_values[0] + "'" + "," + "'" + attr_values[1] + "'" + "," + "'" + attr_values[2] + "'" + "," + "'" + attr_values[3] + "'" + "," + "'" + attr_values[4] + "'" + "," + "'" + attr_values[5] + "'" + "," + "'" + attr_values[6] + "'" + ")" + ";";

    // Debug output
    cout << by_lsoa_query << endl;

    // Run query
    query(pMysql, by_lsoa_query, false);
  }
  else if (int_input == 0)
  {
    return -2;
  }
  return 0;
}

int update_entry(MYSQL *pMysql)
{
  /*
  Update Menu:
    1. Only updates to london_street (london_street is finalized crimes). Technically update is an ADD to london_street.
    2. Ask for all fields 1 by 1. (Verify that fields cant be NULL, error handling)
  */

  /*
  INSERT INTO table_name (column1, column2, column3, ...)
  VALUES (value1, value2, value3, ...);
  */

  // Adding to london_street requires us to first add to london_crime.

  // Add to london_crime
  string table_name = "london_crime ";
  string str_input = "";

  vector<string> l_crime_attributes{"crime_id", "month", "reported_by", "falls_within", "latitude", "longitude", "location", "lsoa_code"};

  vector<string> lcrime_attr_values;

  string add_prefix = "INSERT INTO ";

  cin.ignore();

  for (int i = 0; i < l_crime_attributes.size(); i++)
  {
    cout << "Enter " + l_crime_attributes[i] + ":" << endl;
    //    cin >> str_input;
    getline(cin, str_input);

    lcrime_attr_values.push_back(str_input);
  }

  // Construct Add query given attribute values.
  string lcrime_query = add_prefix + table_name + "(" + "crime_ID, month, reported_by, falls_within, latitude, longitude, location, lsoa_code" + ") " + "VALUES " + "(" + "'" + lcrime_attr_values[0] + "'" + "," + "'" + lcrime_attr_values[1] + "'" + "," + "'" + lcrime_attr_values[2] + "'" + "," + "'" + lcrime_attr_values[3] + "'" + "," + "'" + lcrime_attr_values[4] + "'" + "," + "'" + lcrime_attr_values[5] + "'" + "," + "'" + lcrime_attr_values[6] + "'" + "," + "'" + lcrime_attr_values[7] + "'" + ")" + ";";

  // Debug output
  cout << lcrime_query << endl;

  // Run query
  query(pMysql, lcrime_query, false);

  // Get Auto-incremented offense_ID.
  int offense_ID = query(pMysql, "SELECT MAX(offense_ID) FROM london_crime", true);

  // Add to london_street.
  table_name = "london_street ";
  str_input = "";

  vector<string> l_street_attributes = {"crime_type", "last_outcome_category"};

  vector<string> lstreet_attr_values;

  for (int i = 0; i < l_street_attributes.size(); i++)
  {
    cout << "Enter " + l_street_attributes[i] + ":" << endl;
    //    cin >> str_input;
    getline(cin, str_input);

    lstreet_attr_values.push_back(str_input);
  }

  // Construct Add query given attribute values.
  string lstreet_query = add_prefix + table_name + "(" + "offense_id, crime_type, last_outcome_category" + ") " + "VALUES " + "(" + "'" + to_string(offense_ID) + "'" + "," + "'" + lstreet_attr_values[0] + "'" + "," + "'" + lstreet_attr_values[1] + "'" + ")" + ";";

  // Debug output
  cout << lstreet_query << endl;

  // Run query
  query(pMysql, lstreet_query, false);

  return 0;
}

int search_entry(MYSQL *pMysql)
{
  /*
  Search Menu:
    1. Which table to search.
    2. select * from table where attribute = ...

    london_outcomes filters:
    Month,
    LSOA_code,
    *LSOA_name (reference LSOA_lookup),
    outcome_type

    london_street filters:
    Month,
    LSOA_code,
    *LSOA_name (reference LSOA_lookup),
    crime_type,
    last_outcome_category

    london_crime_by_lsoa filters:
    lsoa_code,
    borough,
    major_category,
    minor_category,
    value,
    year,
    month

    stop_and_search filters:
    gender
    age_range
    self_defined_ethnicity
    officer_defined_ethnicity
    date
    object_of_search
    outcome
  */
  cout << "Which table would you like to search?" << endl;
  cout << "Enter 1 to search london_outcomes." << endl;
  cout << "Enter 2 to search london_street." << endl;
  cout << "Enter 3 to search london_crime_by_lsoa." << endl;
  cout << "Enter 4 to search stop_and_search." << endl;
  cout << "Enter 0 to exit." << endl;

  int input = get_int_input();

  string search_prefix = "SELECT * FROM ";

  if (input == 1)
  {
    // Search london_outcomes.
    string table_name = "((london_outcomes INNER JOIN london_crime ON london_outcomes.offense_ID = london_crime.offense_ID) INNER JOIN LSOA_lookup ON london_crime.lsoa_code = LSOA_lookup.LSOA11CD) ";
    string str_input = "";

    vector<string> filters;
    vector<string> loutcomes_filter_values;
    vector<int> indexes_to_delete;

    int int_input = 0;

    cout << "Which attributes to filter by? Enter all that apply." << endl;
    cout << "Example input: '1224'" << endl;
    cout << "Enter 1 to filter by Year-Month." << endl;
    cout << "Enter 2 to filter by LSOA_Code." << endl;
    cout << "Enter 3 to filter by LSOA_Name." << endl;
    cout << "Enter 4 to filter by outcome_type." << endl;
    cout << "Enter 0 to exit." << endl;

    string filter_input = get_str_input();

    cout << "filter_input: " << filter_input << endl;

    vector<int> filter_choices;

    if (filter_input == "0")
    {
      return -2;
    }
    else
    {
      // Parse input.
      for (int i = 0; i < filter_input.length(); i++)
      {
        char digit = filter_input.at(i);
        if (digit >= 48 && digit <= 57)
        {
          filter_choices.push_back(digit - '0');
        }
      }
    }

    // Sort filter_choices in ascending order.
    sort(filter_choices.begin(), filter_choices.end());

    for (int i = 0; i < filter_choices.size(); i++)
    {
      if (filter_choices.at(i) == 1)
      {
        filters.push_back("month");
      }
      else if (filter_choices.at(i) == 2)
      {
        filters.push_back("lsoa_code");
      }
      else if (filter_choices.at(i) == 3)
      {
        filters.push_back("LSOA11NM");
      }
      else if (filter_choices.at(i) == 4)
      {
        filters.push_back("outcome_type");
      }
      else
      {
        // Invalid input number.
      }
    }

    for (int i = 0; i < filters.size(); i++)
    {
      cout << "Filter by " + filters[i] + "?" << endl;
      if (filters[i].compare("month") == 0)
      {
        cout << "Enter the year to filter by:" << endl;
        string str_input1 = get_str_input();
        cout << "Enter the month to filter by:" << endl;
        cout << "Enter 01 to filter by January." << endl;
        cout << "Enter 02 to filter by Feburary." << endl;
        cout << "Enter 03 to filter by March." << endl;
        cout << "Enter 04 to filter by April." << endl;
        cout << "Enter 05 to filter by May." << endl;
        cout << "Enter 06 to filter by June." << endl;
        cout << "Enter 07 to filter by July." << endl;
        cout << "Enter 08 to filter by August." << endl;
        cout << "Enter 09 to filter by September." << endl;
        cout << "Enter 10 to filter by October." << endl;
        cout << "Enter 11 to filter by November." << endl;
        cout << "Enter 12 to filter by December." << endl;
        string str_input2 = get_str_input();

        string full_date = str_input1 + "-" + str_input2;

        loutcomes_filter_values.push_back(full_date);
      }
      else if (filters[i].compare("lsoa_code") == 0)
      {
        cout << "Enter the LSOA_code to filter by." << endl;
        string str_input = get_str_input();
        loutcomes_filter_values.push_back(str_input);
      }
      else if (filters[i].compare("LSOA11NM") == 0) //from LSOA_lookup table (NEW)
      {
        cout << "Enter the LSOA_name to filter by." << endl;
        //       string str_input = get_str_input();
        string str_input;
        cin.ignore();
        getline(cin, str_input);

        loutcomes_filter_values.push_back(str_input);
      }
      else if (filters[i].compare("outcome_type") == 0)
      {
        cout << "Enter 1 for crimes with the outcome: 'Action to be taken by another organisation'." << endl;
        cout << "Enter 2 for crimes with the outcome: 'Court case unable to proceed'." << endl;
        cout << "Enter 3 for crimes with the outcome: 'Defendant found not guilty'." << endl;
        cout << "Enter 4 for crimes with the outcome: 'Defendant sent to Crown Court'." << endl;
        cout << "Enter 5 for crimes with the outcome: 'Formal action is not in the public interest'." << endl;
        cout << "Enter 6 for crimes with the outcome: 'Further investigation is not in the public interest'." << endl;
        cout << "Enter 7 for crimes with the outcome: 'Investigation complete; no suspect identified'." << endl;
        cout << "Enter 8 for crimes with the outcome: 'Local resolution'." << endl;
        cout << "Enter 9 for crimes with the outcome: 'Offender deprived of property'." << endl;
        cout << "Enter 10 for crimes with the outcome: 'Offender fined'." << endl;
        cout << "Enter 11 for crimes with the outcome: 'Offender given a caution'." << endl;
        cout << "Enter 12 for crimes with the outcome: 'Offender given a drugs possession warning'." << endl;
        cout << "Enter 13 for crimes with the outcome: 'Offender given absolute discharge'." << endl;
        cout << "Enter 14 for crimes with the outcome: 'Offender given community sentence'." << endl;
        cout << "Enter 15 for crimes with the outcome: 'Offender given conditional discharge'." << endl;
        cout << "Enter 16 for crimes with the outcome: 'Offender given penalty notice'." << endl;
        cout << "Enter 17 for crimes with the outcome: 'Offender given suspended prison sentence'." << endl;
        cout << "Enter 18 for crimes with the outcome: 'Offender ordered to pay compensation'." << endl;
        cout << "Enter 19 for crimes with the outcome: 'Offender otherwise dealt with'." << endl;
        cout << "Enter 20 for crimes with the outcome: 'Offender sent to prison'." << endl;
        cout << "Enter 21 for crimes with the outcome: 'Suspect charged'." << endl;
        cout << "Enter 22 for crimes with the outcome: 'Suspect charged as part of another case'." << endl;
        cout << "Enter 23 for crimes with the outcome: 'Unable to prosecute suspect'." << endl;
        cout << "Enter 0 to exit." << endl;

        int_input = get_int_input();

        if (int_input == 1)
        {
          loutcomes_filter_values.push_back("Action to betaken by another organisation");
        }
        else if (int_input == 2)
        {
          loutcomes_filter_values.push_back("Court case unable to proceed");
        }
        else if (int_input == 3)
        {
          loutcomes_filter_values.push_back("Defendant found not guilty");
        }
        else if (int_input == 4)
        {
          loutcomes_filter_values.push_back("Defendant sent to Crown Court");
        }
        else if (int_input == 5)
        {
          loutcomes_filter_values.push_back("Formal action is not in the public interest");
        }
        else if (int_input == 6)
        {
          loutcomes_filter_values.push_back("Further investigation is not in the public interest");
        }
        else if (int_input == 7)
        {
          loutcomes_filter_values.push_back("Investigation complete; no suspect identified");
        }
        else if (int_input == 8)
        {
          loutcomes_filter_values.push_back("Local resolution");
        }
        else if (int_input == 9)
        {
          loutcomes_filter_values.push_back("Offender deprived of property");
        }
        else if (int_input == 10)
        {
          loutcomes_filter_values.push_back("Offender fined");
        }
        else if (int_input == 11)
        {
          loutcomes_filter_values.push_back("Offender given a caution");
        }
        else if (int_input == 12)
        {
          loutcomes_filter_values.push_back("Offender given a drugs possession warning");
        }
        else if (int_input == 13)
        {
          loutcomes_filter_values.push_back("Offender given absolute discharge");
        }
        else if (int_input == 14)
        {
          loutcomes_filter_values.push_back("Offender given community sentence");
        }
        else if (int_input == 15)
        {
          loutcomes_filter_values.push_back("Offender given conditional discharge");
        }
        else if (int_input == 16)
        {
          loutcomes_filter_values.push_back("Offender given penalty notice");
        }
        else if (int_input == 17)
        {
          loutcomes_filter_values.push_back("Offender given suspended prison sentence");
        }
        else if (int_input == 18)
        {
          loutcomes_filter_values.push_back("Offender ordered to pay compensation");
        }
        else if (int_input == 19)
        {
          loutcomes_filter_values.push_back("Offender otherwise dealt with");
        }
        else if (int_input == 20)
        {
          loutcomes_filter_values.push_back("Offender sent to prison");
        }
        else if (int_input == 21)
        {
          loutcomes_filter_values.push_back("Suspect charged");
        }
        else if (int_input == 22)
        {
          loutcomes_filter_values.push_back("Suspect charged as part of another case");
        }
        else if (int_input == 23)
        {
          loutcomes_filter_values.push_back("Unable to prosecute suspect");
        }
        else
        {
          // Invalid input number.
        }
      }
      else
      {
        // Should never get here.
      }
    }

    // Construct Search query given filters and filter values.
    string loutcomes_search_query = search_prefix + table_name + "WHERE ";
    for (int i = 0; i < filters.size(); i++)
    {
      if (i == 0)
      {
        loutcomes_search_query.append("(");
      }
      else if (filter_choices.at(i - 1) != filter_choices.at(i))
      {
        loutcomes_search_query.append("(");
      }
      loutcomes_search_query.append(filters[i]);
      loutcomes_search_query.append(" = ");
      loutcomes_search_query.append("'" + loutcomes_filter_values[i] + "'");
      if (i != filters.size() - 1)
      {
        if (filter_choices.at(i + 1) == filter_choices.at(i))
        {
          loutcomes_search_query.append(" OR ");
        }
        else
        {
          loutcomes_search_query.append(") AND ");
        }
      }
      else
      {
        // Last filter choice, no need to lookahead.
        loutcomes_search_query.append(")");
      }
    }
    loutcomes_search_query.append(";");

    // Debug output.
    cout << loutcomes_search_query << endl;

    // Run the query.
    query(pMysql, loutcomes_search_query, false);
  }
  else if (input == 2)
  {
    // Search london_street.
    /*
    london_street filters:
    Month,
    LSOA_code,
    LSOA_name (reference LSOA_lookup),
    crime_type,
    last_outcome_category
    */

    string table_name = "((london_street INNER JOIN london_crime ON london_street.offense_ID = london_crime.offense_ID) INNER JOIN LSOA_lookup ON london_crime.lsoa_code = LSOA_lookup.LSOA11CD) ";
    string str_input = "";

    vector<string> filters;
    vector<string> lstreet_filter_values;

    int int_input = 0;

    cout << "Which attributes to filter by? Enter all that apply." << endl;
    cout << "Example input: '1224'" << endl;
    cout << "Enter 1 to filter by Year-Month." << endl;
    cout << "Enter 2 to filter by LSOA_Code." << endl;
    cout << "Enter 3 to filter by LSOA_Name." << endl;
    cout << "Enter 4 to filter by crime_type." << endl;
    cout << "Enter 5 to filter by last_outcome_category." << endl;
    cout << "Enter 0 to exit." << endl;

    string filter_input = get_str_input();
    vector<int> filter_choices;

    if (filter_input == "0")
    {
      return -2;
    }
    else
    {
      // Parse input.
      for (int i = 0; i < filter_input.length(); i++)
      {
        char digit = filter_input.at(i);
        if (digit >= 48 && digit <= 57)
        {
          filter_choices.push_back(digit - '0');
        }
      }
    }

    // Sort filter_choices in ascending order.
    sort(filter_choices.begin(), filter_choices.end());

    for (int i = 0; i < filter_choices.size(); i++)
    {
      if (filter_choices.at(i) == 1)
      {
        filters.push_back("month");
      }
      else if (filter_choices.at(i) == 2)
      {
        filters.push_back("lsoa_code");
      }
      else if (filter_choices.at(i) == 3)
      {
        filters.push_back("LSOA11NM");
      }
      else if (filter_choices.at(i) == 4)
      {
        filters.push_back("crime_type");
      }
      else if (filter_choices.at(i) == 5)
      {
        filters.push_back("last_outcome_category");
      }
      else
      {
        // Invalid input number.
      }
    }

    for (int i = 0; i < filters.size(); i++)
    {
      cout << "Filter by " + filters[i] + "?" << endl;
      if (filters[i].compare("month") == 0)
      {
        cout << "Enter the year to filter by:" << endl;
        string str_input1 = get_str_input();
        cout << "Enter the month to filter by:" << endl;
        cout << "Enter 01 to filter by January." << endl;
        cout << "Enter 02 to filter by Feburary." << endl;
        cout << "Enter 03 to filter by March." << endl;
        cout << "Enter 04 to filter by April." << endl;
        cout << "Enter 05 to filter by May." << endl;
        cout << "Enter 06 to filter by June." << endl;
        cout << "Enter 07 to filter by July." << endl;
        cout << "Enter 08 to filter by August." << endl;
        cout << "Enter 09 to filter by September." << endl;
        cout << "Enter 10 to filter by October." << endl;
        cout << "Enter 11 to filter by November." << endl;
        cout << "Enter 12 to filter by December." << endl;
        string str_input2 = get_str_input();

        string full_date = str_input1 + "-" + str_input2;

        lstreet_filter_values.push_back(full_date);
      }
      else if (filters[i].compare("lsoa_code") == 0)
      {
        cout << "Enter the LSOA_code to filter by." << endl;
        string str_input = get_str_input();
        lstreet_filter_values.push_back(str_input);
      }
      else if (filters[i].compare("LSOA11NM") == 0)
      {
        cout << "Enter the LSOA_name to filter by." << endl;
        string str_input;
        cin.ignore();
        getline(cin, str_input);

        lstreet_filter_values.push_back(str_input);
      }
      else if (filters[i].compare("crime_type") == 0)
      {
        /*
        london_street crime_type
        -------------------------------
        Anti-social behaviour
        Bicycle theft
        Burglary
        Criminal damage and arson
        Drugs
        Other crime
        Other theft
        Possession of weapons
        Public order
        Robbery
        Shoplifting
        Theft from the person
        Vehicle crime
        Violence and sexual offences
        */
        cout << "Enter 1 to filter by Anti-social behaviour." << endl;
        cout << "Enter 2 to filter by Bicycle theft." << endl;
        cout << "Enter 3 to filter by Burglary." << endl;
        cout << "Enter 4 to filter by Criminal damage and arson." << endl;
        cout << "Enter 5 to filter by Drugs." << endl;
        cout << "Enter 6 to filter by Other crime." << endl;
        cout << "Enter 7 to filter by Other theft." << endl;
        cout << "Enter 8 to filter by Possession of weapons." << endl;
        cout << "Enter 9 to filter by Public order." << endl;
        cout << "Enter 10 to filter by Robbery." << endl;
        cout << "Enter 11 to filter by Shoplifting." << endl;
        cout << "Enter 12 to filter by Theft from the person." << endl;
        cout << "Enter 13 to filter by Vehicle crime." << endl;
        cout << "Enter 14 to filter by Violence and sexual offences." << endl;

        int_input = get_int_input();

        if (int_input == 1)
        {
          lstreet_filter_values.push_back("Anti-social behaviour");
        }
        else if (int_input == 2)
        {
          lstreet_filter_values.push_back("Bicycle theft");
        }
        else if (int_input == 3)
        {
          lstreet_filter_values.push_back("Burglary");
        }
        else if (int_input == 4)
        {
          lstreet_filter_values.push_back("Criminal damage and arson");
        }
        else if (int_input == 5)
        {
          lstreet_filter_values.push_back("Drugs");
        }
        else if (int_input == 6)
        {
          lstreet_filter_values.push_back("Other crime");
        }
        else if (int_input == 7)
        {
          lstreet_filter_values.push_back("Other theft");
        }
        else if (int_input == 8)
        {
          lstreet_filter_values.push_back("Possession of weapons");
        }
        else if (int_input == 9)
        {
          lstreet_filter_values.push_back("Public order");
        }
        else if (int_input == 10)
        {
          lstreet_filter_values.push_back("Robbery");
        }
        else if (int_input == 11)
        {
          lstreet_filter_values.push_back("Shoplifting");
        }
        else if (int_input == 12)
        {
          lstreet_filter_values.push_back("Theft from the person");
        }
        else if (int_input == 13)
        {
          lstreet_filter_values.push_back("Vehicle crime");
        }
        else if (int_input == 14)
        {
          lstreet_filter_values.push_back("Violence and sexual offences");
        }
        else
        {
          //should never reach here
        }
      }
      else if (filters[i].compare("last_outcome_category") == 0)
      {
        cout << "Enter 1 for crimes with the outcome: 'Action to be taken by another organisation'." << endl;
        cout << "Enter 2 for crimes with the outcome: 'Awaiting court outcome'." << endl;
        cout << "Enter 3 for crimes with the outcome: 'Court case unable to proceed'." << endl;
        cout << "Enter 4 for crimes with the outcome: 'Court result unavailable'." << endl;
        cout << "Enter 5 for crimes with the outcome: 'Defendant found not guilty'." << endl;
        cout << "Enter 6 for crimes with the outcome: 'Defendant sent to Crown Court'." << endl;
        cout << "Enter 7 for crimes with the outcome: 'Formal action is not in the public interest'." << endl;
        cout << "Enter 8 for crimes with the outcome: 'Further investigation is not in the public interest'." << endl;
        cout << "Enter 9 for crimes with the outcome: 'Investigation complete; no suspect identified'." << endl;
        cout << "Enter 10 for crimes with the outcome: 'Local resolution'." << endl;
        cout << "Enter 11 for crimes with the outcome: 'Offender deprived of property'." << endl;
        cout << "Enter 12 for crimes with the outcome: 'Offender fined'." << endl;
        cout << "Enter 13 for crimes with the outcome: 'Offender given a caution'." << endl;
        cout << "Enter 14 for crimes with the outcome: 'Offender given a drugs possession warning'." << endl;
        cout << "Enter 15 for crimes with the outcome: 'Offender given absolute discharge'." << endl;
        cout << "Enter 16 for crimes with the outcome: 'Offender given community sentence'." << endl;
        cout << "Enter 17 for crimes with the outcome: 'Offender given conditional discharge'." << endl;
        cout << "Enter 18 for crimes with the outcome: 'Offender given penalty notice'." << endl;
        cout << "Enter 19 for crimes with the outcome: 'Offender given suspended prison sentence'." << endl;
        cout << "Enter 20 for crimes with the outcome: 'Offender ordered to pay compensation'." << endl;
        cout << "Enter 21 for crimes with the outcome: 'Offender otherwise dealt with'." << endl;
        cout << "Enter 22 for crimes with the outcome: 'Offender sent to prison'." << endl;
        cout << "Enter 23 for crimes with the outcome: 'Status update unavailable'." << endl;
        cout << "Enter 24 for crimes with the outcome: 'Suspect charged as part of another case'." << endl;
        cout << "Enter 25 for crimes with the outcome: 'Unable to prosecute suspect'." << endl;
        cout << "Enter 26 for crimes with the outcome: 'Under investigation'." << endl;
        cout << "Enter 0 to exit." << endl;

        int_input = get_int_input();

        if (int_input == 1)
        {
          lstreet_filter_values.push_back("Action to betaken by another organisation");
        }
        else if (int_input == 2)
        {
          lstreet_filter_values.push_back("Awaiting court outcome");
        }
        else if (int_input == 3)
        {
          lstreet_filter_values.push_back("Court case unable to proceed");
        }
        else if (int_input == 4)
        {
          lstreet_filter_values.push_back("Court result unavailable");
        }
        else if (int_input == 5)
        {
          lstreet_filter_values.push_back("Defendant found not guilty");
        }
        else if (int_input == 6)
        {
          lstreet_filter_values.push_back("Defendant sent to Crown Court");
        }
        else if (int_input == 7)
        {
          lstreet_filter_values.push_back("Formal action is not in the public interest");
        }
        else if (int_input == 8)
        {
          lstreet_filter_values.push_back("Further investigation is not in the public interest");
        }
        else if (int_input == 9)
        {
          lstreet_filter_values.push_back("Investigation complete; no suspect identified");
        }
        else if (int_input == 10)
        {
          lstreet_filter_values.push_back("Local resolution");
        }
        else if (int_input == 11)
        {
          lstreet_filter_values.push_back("Offender deprived of property");
        }
        else if (int_input == 12)
        {
          lstreet_filter_values.push_back("Offender fined");
        }
        else if (int_input == 13)
        {
          lstreet_filter_values.push_back("Offender given a caution");
        }
        else if (int_input == 14)
        {
          lstreet_filter_values.push_back("Offender given a drugs possession warning");
        }
        else if (int_input == 15)
        {
          lstreet_filter_values.push_back("Offender given absolute discharge");
        }
        else if (int_input == 16)
        {
          lstreet_filter_values.push_back("Offender given community sentence");
        }
        else if (int_input == 17)
        {
          lstreet_filter_values.push_back("Offender given conditional discharge");
        }
        else if (int_input == 18)
        {
          lstreet_filter_values.push_back("Offender given penalty notice");
        }
        else if (int_input == 19)
        {
          lstreet_filter_values.push_back("Offender given suspended prison sentence");
        }
        else if (int_input == 20)
        {
          lstreet_filter_values.push_back("Offender ordered to pay compensation");
        }
        else if (int_input == 21)
        {
          lstreet_filter_values.push_back("Offender otherwise dealt with");
        }
        else if (int_input == 22)
        {
          lstreet_filter_values.push_back("Offender sent to prison");
        }
        else if (int_input == 23)
        {
          lstreet_filter_values.push_back("Status update unavailable");
        }
        else if (int_input == 24)
        {
          lstreet_filter_values.push_back("Suspect charged as part of another case");
        }
        else if (int_input == 25)
        {
          lstreet_filter_values.push_back("Unable to prosecute suspect");
        }
        else if (int_input == 26)
        {
          lstreet_filter_values.push_back("Under investigation");
        }
        else
        {
          // Invalid input number.
        }
      }
      else
      {
        // Should never get here.
      }
    }

    // Construct Search query given filters and filter values.
    string lstreet_search_query = search_prefix + table_name + "WHERE ";
    for (int i = 0; i < filters.size(); i++)
    {
      if (i == 0)
      {
        lstreet_search_query.append("(");
      }
      else if (filter_choices.at(i - 1) != filter_choices.at(i))
      {
        lstreet_search_query.append("(");
      }
      lstreet_search_query.append(filters[i]);
      lstreet_search_query.append(" = ");
      lstreet_search_query.append("'" + lstreet_filter_values[i] + "'");
      if (i != filters.size() - 1)
      {
        if (filter_choices.at(i + 1) == filter_choices.at(i))
        {
          lstreet_search_query.append(" OR ");
        }
        else
        {
          lstreet_search_query.append(") AND ");
        }
      }
      else
      {
        // Last filter choice, no need to lookahead.
        lstreet_search_query.append(")");
      }
    }
    lstreet_search_query.append(";");

    // Debug output.
    cout << lstreet_search_query << endl;

    // Run the query.
    query(pMysql, lstreet_search_query, false);
  }
  else if (input == 3)
  {
    // Search london_crime_by_lsoa.
    /*
    london_crime_by_lsoa filters:
    lsoa_code,
    borough,
    major_category,
    minor_category,
    year,
    month
    */

    string table_name = "london_crime_by_lsoa ";
    string str_input = "";

    vector<string> filters;
    vector<string> by_lsoa_filter_values;

    int int_input = 0;

    cout << "Which attributes to filter by? Enter all that apply." << endl;
    cout << "Example input: '1224'" << endl;
    cout << "Enter 1 to filter by LSOA_Code." << endl;
    cout << "Enter 2 to filter by borough." << endl;
    cout << "Enter 3 to filter by major_category." << endl;
    cout << "Enter 4 to filter by minor_category." << endl;
    cout << "Enter 5 to filter by year." << endl;
    cout << "Enter 6 to filter by month." << endl;
    cout << "Enter 0 to exit." << endl;

    string filter_input = get_str_input();
    vector<int> filter_choices;

    if (filter_input == "0")
    {
      return -2;
    }
    else
    {
      // Parse input.
      for (int i = 0; i < filter_input.length(); i++)
      {
        char digit = filter_input.at(i);
        if (digit >= 48 && digit <= 57)
        {
          filter_choices.push_back(digit - '0');
        }
      }
    }

    // Sort filter_choices in ascending order.
    sort(filter_choices.begin(), filter_choices.end());

    for (int i = 0; i < filter_choices.size(); i++)
    {
      if (filter_choices.at(i) == 1)
      {
        filters.push_back("lsoa_code");
      }
      else if (filter_choices.at(i) == 2)
      {
        filters.push_back("borough");
      }
      else if (filter_choices.at(i) == 3)
      {
        filters.push_back("major_category");
      }
      else if (filter_choices.at(i) == 4)
      {
        filters.push_back("minor_category");
      }
      else if (filter_choices.at(i) == 5)
      {
        filters.push_back("year");
      }
      else if (filter_choices.at(i) == 6)
      {
        filters.push_back("month");
      }
      else
      {
        // Invalid input number.
      }
    }

    for (int i = 0; i < filters.size(); i++)
    {
      cout << "Filter by " + filters[i] + "?" << endl;
      if (filters[i].compare("lsoa_code") == 0)
      {
        cout << "Enter the LSOA_code to filter by." << endl;
        string str_input = get_str_input();
        by_lsoa_filter_values.push_back(str_input);
      }
      else if (filters[i].compare("borough") == 0)
      {
        /*
        Croydon
        Greenwich
        Bromley
        Redbridge
        Wandsworth
        Ealing
        Hounslow
        Newham
        Sutton
        Haringey
        Lambeth
        Richmond upon Thames
        Hillingdon
        Havering
        Barking and Dagenham
        Kingston upon Thames
        Westminster
        Hacknev
        Enfield
        Harrow
        Lewisham
        Brent
        Southwark
        Barnet
        Waltham Forest
        Camden
        Bexley
        Kensington and Chelsea
        Islington
        Tower Hamlets
        Hammersmith and Fulham
        Merton
        City of London
        */
        cout << "Enter 1 to filter by Croydon." << endl;
        cout << "Enter 2 to filter by Greenwich." << endl;
        cout << "Enter 3 to filter by Bromley." << endl;
        cout << "Enter 4 to filter by Redbridge." << endl;
        cout << "Enter 5 to filter by Wandsworth." << endl;
        cout << "Enter 6 to filter by Ealing." << endl;
        cout << "Enter 7 to filter by Hounslow." << endl;
        cout << "Enter 8 to filter by Newham." << endl;
        cout << "Enter 9 to filter by Sutton." << endl;
        cout << "Enter 10 to filter by Haringey." << endl;
        cout << "Enter 11 to filter by Lambeth." << endl;
        cout << "Enter 12 to filter by Richmond upon Thames." << endl;
        cout << "Enter 13 to filter by Hillingdon." << endl;
        cout << "Enter 14 to filter by Havering." << endl;
        cout << "Enter 15 to filter by Barking and Dagenham." << endl;
        cout << "Enter 16 to filter by Kingston upon Thames." << endl;
        cout << "Enter 17 to filter by Westminster." << endl;
        cout << "Enter 18 to filter by Hacknev." << endl;
        cout << "Enter 19 to filter by Enfield." << endl;
        cout << "Enter 20 to filter by Harrow." << endl;
        cout << "Enter 21 to filter by Lewisham." << endl;
        cout << "Enter 22 to filter by Brent." << endl;
        cout << "Enter 23 to filter by Southwark." << endl;
        cout << "Enter 24 to filter by Barnet." << endl;
        cout << "Enter 25 to filter by Waltham Forest." << endl;
        cout << "Enter 26 to filter by Camden." << endl;
        cout << "Enter 27 to filter by Bexley." << endl;
        cout << "Enter 28 to filter by Kensington and Chelsea." << endl;
        cout << "Enter 29 to filter by Islington." << endl;
        cout << "Enter 30 to filter by Tower Hamlets." << endl;
        cout << "Enter 31 to filter by Hammersmith and Fulham." << endl;
        cout << "Enter 32 to filter by Merton." << endl;
        cout << "Enter 33 to filter by City of London." << endl;

        int int_input = get_int_input();

        if (int_input == 1)
        {
          by_lsoa_filter_values.push_back("Croydon");
        }
        else if (int_input == 2)
        {
          by_lsoa_filter_values.push_back("Greenwich");
        }
        else if (int_input == 3)
        {
          by_lsoa_filter_values.push_back("Bromley");
        }
        else if (int_input == 4)
        {
          by_lsoa_filter_values.push_back("Redbridge");
        }
        else if (int_input == 5)
        {
          by_lsoa_filter_values.push_back("Wandsworth");
        }
        else if (int_input == 6)
        {
          by_lsoa_filter_values.push_back("Ealing");
        }
        else if (int_input == 7)
        {
          by_lsoa_filter_values.push_back("Hounslow");
        }
        else if (int_input == 8)
        {
          by_lsoa_filter_values.push_back("Newham");
        }
        else if (int_input == 9)
        {
          by_lsoa_filter_values.push_back("Sutton");
        }
        else if (int_input == 10)
        {
          by_lsoa_filter_values.push_back("Haringey");
        }
        else if (int_input == 11)
        {
          by_lsoa_filter_values.push_back("Lambeth");
        }
        else if (int_input == 12)
        {
          by_lsoa_filter_values.push_back("Richmond upon Thames");
        }
        else if (int_input == 13)
        {
          by_lsoa_filter_values.push_back("Hillingdon");
        }
        else if (int_input == 14)
        {
          by_lsoa_filter_values.push_back("Havering");
        }
        else if (int_input == 15)
        {
          by_lsoa_filter_values.push_back("Barking and Dagenham");
        }
        else if (int_input == 16)
        {
          by_lsoa_filter_values.push_back("Kingston upon Thames");
        }
        else if (int_input == 17)
        {
          by_lsoa_filter_values.push_back("Westminster");
        }
        else if (int_input == 18)
        {
          by_lsoa_filter_values.push_back("Hackney");
        }
        else if (int_input == 19)
        {
          by_lsoa_filter_values.push_back("Enfield");
        }
        else if (int_input == 20)
        {
          by_lsoa_filter_values.push_back("Harrow");
        }
        else if (int_input == 21)
        {
          by_lsoa_filter_values.push_back("Lewisham");
        }
        else if (int_input == 22)
        {
          by_lsoa_filter_values.push_back("Brent");
        }
        else if (int_input == 23)
        {
          by_lsoa_filter_values.push_back("Southwark");
        }
        else if (int_input == 24)
        {
          by_lsoa_filter_values.push_back("Barnet");
        }
        else if (int_input == 25)
        {
          by_lsoa_filter_values.push_back("Waltham Forest");
        }
        else if (int_input == 26)
        {
          by_lsoa_filter_values.push_back("Camden");
        }
        else if (int_input == 27)
        {
          by_lsoa_filter_values.push_back("Bexley");
        }
        else if (int_input == 28)
        {
          by_lsoa_filter_values.push_back("Kensington and Chelsea");
        }
        else if (int_input == 29)
        {
          by_lsoa_filter_values.push_back("Islington");
        }
        else if (int_input == 30)
        {
          by_lsoa_filter_values.push_back("Tower Hamlets");
        }
        else if (int_input == 31)
        {
          by_lsoa_filter_values.push_back("Hammersmith and Fulham");
        }
        else if (int_input == 32)
        {
          by_lsoa_filter_values.push_back("Merton");
        }
        else if (int_input == 33)
        {
          by_lsoa_filter_values.push_back("City of London");
        }
      }
      else if (filters[i].compare("major_category") == 0)
      {
        /*
        Major Categories
        ----------------------------
        Burglary
        Criminal Damage
        Drugs
        Fraud or Forgery
        Other Notifiable Offences
        Robbery
        Sexual Offences
        Theft and Handling
        Violence Against the Person
        */
        cout << "Enter 1 to filter by Burglary." << endl;
        cout << "Enter 2 to filter by Criminal Damage." << endl;
        cout << "Enter 3 to filter by Drugs." << endl;
        cout << "Enter 4 to filter by Fraud or Forgery." << endl;
        cout << "Enter 5 to filter by Other Notifiable Offences." << endl;
        cout << "Enter 6 to filter by Robbery." << endl;
        cout << "Enter 7 to filter by Sexual Offences." << endl;
        cout << "Enter 8 to filter by Theft and Handling." << endl;
        cout << "Enter 9 to filter by Violence Against the Person." << endl;

        int int_input = get_int_input();

        if (int_input == 1)
        {
          by_lsoa_filter_values.push_back("Burglary");
        }
        else if (int_input == 2)
        {
          by_lsoa_filter_values.push_back("Criminal Damage");
        }
        else if (int_input == 3)
        {
          by_lsoa_filter_values.push_back("Drugs");
        }
        else if (int_input == 4)
        {
          by_lsoa_filter_values.push_back("Fraud or Forgery");
        }
        else if (int_input == 5)
        {
          by_lsoa_filter_values.push_back("Other Notifiable Offences");
        }
        else if (int_input == 6)
        {
          by_lsoa_filter_values.push_back("Robbery");
        }
        else if (int_input == 7)
        {
          by_lsoa_filter_values.push_back("Sexual Offences");
        }
        else if (int_input == 8)
        {
          by_lsoa_filter_values.push_back("Theft and Handling");
        }
        else if (int_input == 9)
        {
          by_lsoa_filter_values.push_back("Violence Against the Person");
        }
      }
      else if (filters[i].compare("minor_category") == 0)
      {
        /*
        Minor Categories
        -----------------------------
        Assault with Injury
        Burglary in a Dwelling
        Burglary in Other Buildings
        Business Property
        Common Assault
        Counted per Victim
        Criminal Damage To Dwelling
        Criminal Damage To Motor Vehicle
        Criminal Damage To Other Building
        Drug Trafficking
        Going Equipped
        Handling Stolen Goods
        Harassment
        Motor Vehicle Interference & Tampering
        Murder
        Offensive Weapon
        Other Criminal Damage
        Other Drugs
        Other Fraud & Forgery
        Other Notifiable
        Other Sexual
        Other Theft
        Other Theft Person
        Other violence
        Personal Property
        Possession Of Drugs
        Rape
        Theft From Motor Vehicle
        Theft From Shops
        Theft/Taking Of Motor Vehicle
        Theft/Taking of Pedal Cycle
        Wounding/GBH
        */
        cout << "Enter 1 to filter by Assault with Injury." << endl;
        cout << "Enter 2 to filter by Burglary in a Dwelling." << endl;
        cout << "Enter 3 to filter by Burglary in Other Buildings." << endl;
        cout << "Enter 4 to filter by Business Property." << endl;
        cout << "Enter 5 to filter by Common Assault." << endl;
        cout << "Enter 6 to filter by Counted per Victim." << endl;
        cout << "Enter 7 to filter by Criminal Damage To Dwelling." << endl;
        cout << "Enter 8 to filter by Criminal Damage To Motor Vehicle." << endl;
        cout << "Enter 9 to filter by Criminal Damage To Other Building." << endl;
        cout << "Enter 10 to filter by Drug Trafficking." << endl;
        cout << "Enter 11 to filter by Going Equipped." << endl;
        cout << "Enter 12 to filter by Handling Stolen Goods." << endl;
        cout << "Enter 13 to filter by Harassment." << endl;
        cout << "Enter 14 to filter by Motor Vehicle Interference & Tampering." << endl;
        cout << "Enter 15 to filter by Murder." << endl;
        cout << "Enter 16 to filter by Offensive Weapon." << endl;
        cout << "Enter 17 to filter by Other Criminal Damage." << endl;
        cout << "Enter 18 to filter by Other Drugs." << endl;
        cout << "Enter 19 to filter by Other Fraud & Forgery." << endl;
        cout << "Enter 20 to filter by Other Notifiable." << endl;
        cout << "Enter 21 to filter by Other Sexual." << endl;
        cout << "Enter 22 to filter by Other Theft." << endl;
        cout << "Enter 23 to filter by Other Theft Person." << endl;
        cout << "Enter 24 to filter by Other violence." << endl;
        cout << "Enter 25 to filter by Personal Property." << endl;
        cout << "Enter 26 to filter by Possession Of Drugs." << endl;
        cout << "Enter 27 to filter by Rape." << endl;
        cout << "Enter 28 to filter by Theft From Motor Vehicle." << endl;
        cout << "Enter 29 to filter by Theft From Shops." << endl;
        cout << "Enter 30 to filter by Theft/Taking Of Motor Vehicle." << endl;
        cout << "Enter 31 to filter by Theft/Taking of Pedal Cycle." << endl;
        cout << "Enter 32 to filter by Wounding/GBH." << endl;

        int int_input = get_int_input();

        if (int_input == 1)
        {
          by_lsoa_filter_values.push_back("Assault with Injury");
        }
        else if (int_input == 2)
        {
          by_lsoa_filter_values.push_back("Burglary in a Dwelling");
        }
        else if (int_input == 3)
        {
          by_lsoa_filter_values.push_back("Burglary in Other Buildings");
        }
        else if (int_input == 4)
        {
          by_lsoa_filter_values.push_back("Business Property");
        }
        else if (int_input == 5)
        {
          by_lsoa_filter_values.push_back("Common Assault");
        }
        else if (int_input == 6)
        {
          by_lsoa_filter_values.push_back("Counted per Victim");
        }
        else if (int_input == 7)
        {
          by_lsoa_filter_values.push_back("Criminal Damage To Dwelling");
        }
        else if (int_input == 8)
        {
          by_lsoa_filter_values.push_back("Criminal Damage To Motor Vehicle");
        }
        else if (int_input == 9)
        {
          by_lsoa_filter_values.push_back("Criminal Damage To Other Building");
        }
        else if (int_input == 10)
        {
          by_lsoa_filter_values.push_back("Drug Trafficking");
        }
        else if (int_input == 11)
        {
          by_lsoa_filter_values.push_back("Going Equipped");
        }
        else if (int_input == 12)
        {
          by_lsoa_filter_values.push_back("Handling Stolen Goods");
        }
        else if (int_input == 13)
        {
          by_lsoa_filter_values.push_back("Harassment");
        }
        else if (int_input == 14)
        {
          by_lsoa_filter_values.push_back("Motor Vehicle Interference & Tampering");
        }
        else if (int_input == 15)
        {
          by_lsoa_filter_values.push_back("Murder");
        }
        else if (int_input == 16)
        {
          by_lsoa_filter_values.push_back("Offensive Weapon");
        }
        else if (int_input == 17)
        {
          by_lsoa_filter_values.push_back("Other Criminal Damage");
        }
        else if (int_input == 18)
        {
          by_lsoa_filter_values.push_back("Other Drugs");
        }
        else if (int_input == 19)
        {
          by_lsoa_filter_values.push_back("Other Fraud & Forgery");
        }
        else if (int_input == 20)
        {
          by_lsoa_filter_values.push_back("Other Notifiable");
        }
        else if (int_input == 21)
        {
          by_lsoa_filter_values.push_back("Other Sexual");
        }
        else if (int_input == 22)
        {
          by_lsoa_filter_values.push_back("Other Theft");
        }
        else if (int_input == 23)
        {
          by_lsoa_filter_values.push_back("Other Theft Person");
        }
        else if (int_input == 24)
        {
          by_lsoa_filter_values.push_back("Other violence");
        }
        else if (int_input == 25)
        {
          by_lsoa_filter_values.push_back("Personal Property");
        }
        else if (int_input == 26)
        {
          by_lsoa_filter_values.push_back("Possession Of Drugs");
        }
        else if (int_input == 27)
        {
          by_lsoa_filter_values.push_back("Rape");
        }
        else if (int_input == 28)
        {
          by_lsoa_filter_values.push_back("Theft From Motor Vehicle");
        }
        else if (int_input == 29)
        {
          by_lsoa_filter_values.push_back("Theft From Shops");
        }
        else if (int_input == 30)
        {
          by_lsoa_filter_values.push_back("Theft/Taking Of Motor Vehicle");
        }
        else if (int_input == 31)
        {
          by_lsoa_filter_values.push_back("Theft/Taking of Pedal Cycle");
        }
        else if (int_input == 32)
        {
          by_lsoa_filter_values.push_back("Wounding/GBH");
        }
        else
        {
          // Invalid input number.
        }
      }
      else if (filters[i].compare("year") == 0)
      {
        cout << "Enter the year to filter by:" << endl;
        string str_input = get_str_input();
        by_lsoa_filter_values.push_back(str_input);
      }
      else if (filters[i].compare("month") == 0)
      {
        cout << "Enter the month to filter by:" << endl;
        cout << "Enter 1 to filter by January." << endl;
        cout << "Enter 2 to filter by Feburary." << endl;
        cout << "Enter 3 to filter by March." << endl;
        cout << "Enter 4 to filter by April." << endl;
        cout << "Enter 5 to filter by May." << endl;
        cout << "Enter 6 to filter by June." << endl;
        cout << "Enter 7 to filter by July." << endl;
        cout << "Enter 8 to filter by August." << endl;
        cout << "Enter 9 to filter by September." << endl;
        cout << "Enter 10 to filter by October." << endl;
        cout << "Enter 11 to filter by November." << endl;
        cout << "Enter 12 to filter by December." << endl;
        string str_input = get_str_input();
        by_lsoa_filter_values.push_back(str_input);
      }
      else
      {
        // Should never get here.
      }
    }

    // Debug Print Vectors
    cout << filters.size() << endl;
    for (auto i : filters)
    {
      cout << i << endl;
    }
    cout << filter_choices.size() << endl;
    for (auto i : filter_choices)
    {
      cout << i << endl;
    }
    cout << by_lsoa_filter_values.size() << endl;
    for (auto i : by_lsoa_filter_values)
    {
      cout << i << endl;
    }

    // Construct Search query given filters and filter values.
    string by_lsoa_search_query = search_prefix + table_name + "WHERE ";
    for (int i = 0; i < filters.size(); i++)
    {
      if (i == 0)
      {
        by_lsoa_search_query.append("(");
      }
      else if (filter_choices.at(i - 1) != filter_choices.at(i))
      {
        by_lsoa_search_query.append("(");
      }
      by_lsoa_search_query.append(filters[i]);
      by_lsoa_search_query.append(" = ");
      by_lsoa_search_query.append("'" + by_lsoa_filter_values[i] + "'");
      if (i != filters.size() - 1)
      {
        if (filter_choices.at(i + 1) == filter_choices.at(i))
        {
          by_lsoa_search_query.append(" OR ");
        }
        else
        {
          by_lsoa_search_query.append(") AND ");
        }
      }
      else
      {
        // Last filter choice, no need to lookahead.
        by_lsoa_search_query.append(")");
      }
    }
    by_lsoa_search_query.append(";");

    // Debug output.
    cout << by_lsoa_search_query << endl;

    // Run the query.
    query(pMysql, by_lsoa_search_query, false);
  }
  else if (input == 4)
  {
    // Search stop_and_search.
    /*
    stop_and_search filters:
    gender
    age_range
    self_defined_ethnicity
    officer_defined_ethnicity
    date
    latitude
    longitude
    object_of_search
    outcome
    */

    string table_name = "stop_and_search ";
    string str_input = "";

    vector<string> filters;
    vector<string> sas_filter_values;

    int int_input = 0;

    cout << "Which attributes to filter by? Enter all that apply." << endl;
    cout << "Example input: '1224'" << endl;
    cout << "Enter 1 to filter by gender." << endl;
    cout << "Enter 2 to filter by age_range." << endl;
    cout << "Enter 3 to filter by self_defined_ethnicity." << endl;
    cout << "Enter 4 to filter by officer_defined_ethnicity." << endl;
    cout << "Enter 5 to filter by date." << endl;
    cout << "Enter 6 to filter by object_of_search." << endl;
    cout << "Enter 7 to filter by outcome." << endl;
    cout << "Enter 0 to exit." << endl;

    string filter_input = get_str_input();
    vector<int> filter_choices;

    if (filter_input == "0")
    {
      return -2;
    }
    else
    {
      // Parse input.
      for (int i = 0; i < filter_input.length(); i++)
      {
        char digit = filter_input.at(i);
        if (digit >= 48 && digit <= 57)
        {
          filter_choices.push_back(digit - '0');
        }
      }
    }

    // Sort filter_choices in ascending order.
    sort(filter_choices.begin(), filter_choices.end());

    for (int i = 0; i < filter_choices.size(); i++)
    {
      if (filter_choices.at(i) == 1)
      {
        filters.push_back("gender");
      }
      else if (filter_choices.at(i) == 2)
      {
        filters.push_back("age_range");
      }
      else if (filter_choices.at(i) == 3)
      {
        filters.push_back("self_defined_ethnicity");
      }
      else if (filter_choices.at(i) == 4)
      {
        filters.push_back("officer_defined_ethnicity");
      }
      else if (filter_choices.at(i) == 5)
      {
        filters.push_back("date");
      }
      else if (filter_choices.at(i) == 6)
      {
        filters.push_back("object_of_search");
      }
      else if (filter_choices.at(i) == 7)
      {
        filters.push_back("outcome");
      }
      else
      {
        // Invalid input number.
      }
    }

    for (int i = 0; i < filters.size(); i++)
    {
      cout << "Filter by " + filters[i] + "?" << endl;
      if (filters[i].compare("gender") == 0)
      {
        cout << "Enter 1 to filter by Male." << endl;
        cout << "Enter 2 to filter by Female." << endl;
        cout << "Enter 3 to filter by Other." << endl;

        int int_input = get_int_input();

        if (int_input == 1)
        {
          sas_filter_values.push_back("Male");
        }
        else if (int_input == 2)
        {
          sas_filter_values.push_back("Female");
        }
        else if (int_input == 3)
        {
          sas_filter_values.push_back("Other");
        }
      }
      else if (filters[i].compare("age_range") == 0)
      {
        cout << "Enter 1 to filter by Under 10." << endl;
        cout << "Enter 2 to filter by 10-17." << endl;
        cout << "Enter 3 to filter by 18-24." << endl;
        cout << "Enter 4 to filter by 25-34." << endl;
        cout << "Enter 5 to filter by Over 34." << endl;

        int int_input = get_int_input();

        if (int_input == 1)
        {
          sas_filter_values.push_back("under 10");
        }
        else if (int_input == 2)
        {
          sas_filter_values.push_back("10-17");
        }
        else if (int_input == 3)
        {
          sas_filter_values.push_back("18-24");
        }
        else if (int_input == 4)
        {
          sas_filter_values.push_back("25-34");
        }
        else if (int_input == 5)
        {
          sas_filter_values.push_back("over 34");
        }
      }
      else if (filters[i].compare("self_defined_ethnicity") == 0)
      {
        /*
        Asian or Asian British Bangladeshi (A3)
        White Any other White ethnic background (W9)
        White White British (W1)
        Asian or Asian British Any other Asian ethnic background (A9)
        Not Stated (NS)
        Mixed Any other Mixed ethnic background (M9)
        Black or Black British African (B2)
        Chinese or other ethnic group Chinese (01)
        Mixed White and Black Caribbean (M1)
        Black or Black British Any other Black ethnic background (B9)
        Black or Black British Caribbean (B1)
        Asian or Asian British Indian (A1)
        Asian or Asian British Pakistani (A2)
        Mixed White and Asian (M3)
        Mixed White and Black African (M2)
        white White Irish (W2)
        Chinese or other ethnic group Any other ethnic group (09)
        */
        cout << "Enter 1 to filter by 'Asian or Asian British - Bangladeshi (A3)'." << endl;
        cout << "Enter 2 to filter by 'White - Any other White ethnic background (W9)'." << endl;
        cout << "Enter 3 to filter by 'White - White British (W1)'." << endl;
        cout << "Enter 4 to filter by 'Asian or Asian British - Any other Asian ethnic background (A9)'." << endl;
        cout << "Enter 5 to filter by 'Not Stated (NS)'." << endl;
        cout << "Enter 6 to filter by 'Mixed - Any other Mixed ethnic background (M9)'." << endl;
        cout << "Enter 7 to filter by 'Black or Black British - African (B2)'." << endl;
        cout << "Enter 8 to filter by 'Chinese or other ethnic group - Chinese (01)'." << endl;
        cout << "Enter 9 to filter by 'Mixed - White and Black Caribbean (M1)'." << endl;
        cout << "Enter 10 to filter by 'Black or Black British - Any other Black ethnic background (B9)'." << endl;
        cout << "Enter 11 to filter by 'Black or Black British - Caribbean (B1)'." << endl;
        cout << "Enter 12 to filter by 'Asian or Asian British - Indian (A1)'." << endl;
        cout << "Enter 13 to filter by 'Asian or Asian British - Pakistani (A2)'." << endl;
        cout << "Enter 14 to filter by 'Mixed - White and Asian (M3)'." << endl;
        cout << "Enter 15 to filter by 'Mixed - White and Black African (M2)'." << endl;
        cout << "Enter 16 to filter by 'White - White Irish (W2)'." << endl;
        cout << "Enter 17 to filter by 'Chinese or other ethnic group - Any other ethnic group (09)'." << endl;

        int int_input = get_int_input();

        if (int_input == 1)
        {
          sas_filter_values.push_back("Asian or Asian British - Bangladeshi (A3)");
        }
        else if (int_input == 2)
        {
          sas_filter_values.push_back("White - Any other White ethnic background (W9)");
        }
        else if (int_input == 3)
        {
          sas_filter_values.push_back("White - White British (W1)");
        }
        else if (int_input == 4)
        {
          sas_filter_values.push_back("Asian or Asian British - Any other Asian ethnic background (A9)");
        }
        else if (int_input == 5)
        {
          sas_filter_values.push_back("Not Stated (NS)");
        }
        else if (int_input == 6)
        {
          sas_filter_values.push_back("Mixed - Any other Mixed ethnic background (M9)");
        }
        else if (int_input == 7)
        {
          sas_filter_values.push_back("Black or Black British - African (B2)");
        }
        else if (int_input == 8)
        {
          sas_filter_values.push_back("Chinese or other ethnic group - Chinese (01)");
        }
        else if (int_input == 9)
        {
          sas_filter_values.push_back("Mixed - White and Black Caribbean (M1)");
        }
        else if (int_input == 10)
        {
          sas_filter_values.push_back("Black or Black British - Any other Black ethnic background (B9)");
        }
        else if (int_input == 11)
        {
          sas_filter_values.push_back("Black or Black British - Caribbean (B1)");
        }
        else if (int_input == 12)
        {
          sas_filter_values.push_back("Asian or Asian British - Indian (A1)");
        }
        else if (int_input == 13)
        {
          sas_filter_values.push_back("Asian or Asian British - Pakistani (A2)");
        }
        else if (int_input == 14)
        {
          sas_filter_values.push_back("Mixed - White and Asian (M3)");
        }
        else if (int_input == 15)
        {
          sas_filter_values.push_back("Mixed - White and Black African (M2)");
        }
        else if (int_input == 16)
        {
          sas_filter_values.push_back("White - White Irish (W2)");
        }
        else if (int_input == 17)
        {
          sas_filter_values.push_back("Chinese or other ethnic group - Any other ethnic group (09)");
        }
      }
      else if (filters[i].compare("officer_defined_ethnicity") == 0)
      {
        /*
        Asian
        White
        Black
        Other
        Mixed
        */
        cout << "Enter 1 to filter by 'Asian'." << endl;
        cout << "Enter 2 to filter by 'White'." << endl;
        cout << "Enter 3 to filter by 'Black'." << endl;
        cout << "Enter 4 to filter by 'Other'." << endl;
        cout << "Enter 5 to filter by 'Mixed'." << endl;

        int int_input = get_int_input();

        if (int_input == 1)
        {
          sas_filter_values.push_back("Asian");
        }
        else if (int_input == 2)
        {
          sas_filter_values.push_back("White");
        }
        else if (int_input == 3)
        {
          sas_filter_values.push_back("Black");
        }
        else if (int_input == 4)
        {
          sas_filter_values.push_back("Other");
        }
        else if (int_input == 5)
        {
          sas_filter_values.push_back("Mixed");
        }
      }
      else if (filters[i].compare("date") == 0)
      {
        cout << "Enter the YYYY-MM-DD to filter by:" << endl;
        string str_input = get_str_input();
        sas_filter_values.push_back(str_input);
      }
      else if (filters[i].compare("object_of_search") == 0)
      {
        cout << "Enter 1 to filter by Stolen goods." << endl;
        cout << "Enter 2 to filter by Controlled drugs." << endl;
        cout << "Enter 3 to filter by Article for use in theft." << endl;
        cout << "Enter 4 to filter by Offensive weapons." << endl;
        cout << "Enter 5 to filter by Articles for use in criminal damage." << endl;
        cout << "Enter 6 to filter by Fireworks." << endl;
        cout << "Enter 7 to filter by Evidence of offences under the Act." << endl;
        cout << "Enter 8 to filter by Anything to threaten or harm anyone." << endl;
        cout << "Enter 9 to filter by Firearms." << endl;

        int int_input = get_int_input();

        if (int_input == 1)
        {
          sas_filter_values.push_back("Stolen goods");
        }
        else if (int_input == 2)
        {
          sas_filter_values.push_back("Controlled drugs");
        }
        else if (int_input == 3)
        {
          sas_filter_values.push_back("Article for use in theft");
        }
        else if (int_input == 4)
        {
          sas_filter_values.push_back("Offensive weapons");
        }
        else if (int_input == 5)
        {
          sas_filter_values.push_back("Articles for use in criminal damage");
        }
        else if (int_input == 6)
        {
          sas_filter_values.push_back("Fireworks");
        }
        else if (int_input == 7)
        {
          sas_filter_values.push_back("Evidence of offences under the Act");
        }
        else if (int_input == 8)
        {
          sas_filter_values.push_back("Anything to threaten or harm anyone");
        }
        else if (int_input == 9)
        {
          sas_filter_values.push_back("Firearms");
        }
      }
      else if (filters[i].compare("outcome") == 0)
      {
        cout << "Enter 1 to filter by 'Article found - Detailed outcome unavailable'." << endl;
        cout << "Enter 2 to filter by 'Local resolution'." << endl;
        cout << "Enter 3 to filter by 'Nothing found - no further action'." << endl;
        cout << "Enter 4 to filter by 'Offender cautioned'." << endl;
        cout << "Enter 5 to filter by 'Offender given drugs possession warning'." << endl;
        cout << "Enter 6 to filter by 'Offender given penalty notice'." << endl;
        cout << "Enter 7 to filter by 'Suspect arrested'." << endl;
        cout << "Enter 8 to filter by 'Suspect summonsed to court'." << endl;

        int int_input = get_int_input();

        if (int_input == 1)
        {
          sas_filter_values.push_back("Article found - Detailed outcome unavailable");
        }
        else if (int_input == 2)
        {
          sas_filter_values.push_back("Local resolution");
        }
        else if (int_input == 3)
        {
          sas_filter_values.push_back("Nothing found - no further action");
        }
        else if (int_input == 4)
        {
          sas_filter_values.push_back("Offender cautioned");
        }
        else if (int_input == 5)
        {
          sas_filter_values.push_back("Offender given drugs possession warning");
        }
        else if (int_input == 6)
        {
          sas_filter_values.push_back("Offender given penalty notice");
        }
        else if (int_input == 7)
        {
          sas_filter_values.push_back("Suspect arrested");
        }
        else if (int_input == 8)
        {
          sas_filter_values.push_back("Suspect summonsed to court");
        }
      }
      else
      {
        // Should never get here.
      }
    }

    string sas_search_query = search_prefix + table_name + "WHERE ";
    for (int i = 0; i < filters.size(); i++)
    {
      if (i == 0)
      {
        sas_search_query.append("(");
      }
      else if (filter_choices.at(i - 1) != filter_choices.at(i))
      {
        sas_search_query.append("(");
      }
      sas_search_query.append(filters[i]);
      if (filters[i].compare("date") == 0)
      {
        sas_search_query.append(" LIKE ");
        sas_search_query.append("'" + sas_filter_values[i]);
        sas_search_query.append("%'");
      }
      else
      {
        sas_search_query.append(" = ");
        sas_search_query.append("'" + sas_filter_values[i] + "'");
      }
      if (i != filters.size() - 1)
      {
        if (filter_choices.at(i + 1) == filter_choices.at(i))
        {
          sas_search_query.append(" OR ");
        }
        else
        {
          sas_search_query.append(") AND ");
        }
      }
      else
      {
        // Last filter choice, no need to lookahead.
        sas_search_query.append(")");
      }
    }
    sas_search_query.append(";");

    // Debug output.
    cout << sas_search_query << endl;

    // Run the query.
    query(pMysql, sas_search_query, false);
  }
  else if (input == 0)
  {
    return -2;
  }
  return 0;
}

int h_or_l_crime_lsoa(MYSQL *pMysql)
{
  int h_or_l_input;
  cout << "Enter 1 to search for LSOAs with the HIGHEST crime." << endl;
  cout << "Enter 2 to search for LSOAs with the LOWEST crime." << endl;
  cin >> h_or_l_input;

  int limit_input;
  cout << "How many entries do you want to see?" << endl;
  cin >> limit_input;

  string select = "SELECT lsoa_code, LSOA11NM, COUNT(lsoa_code) AS `#crimes` ";
  string from = "FROM (london_crime_by_lsoa INNER JOIN LSOA_lookup ON london_crime_by_lsoa.lsoa_code = LSOA_lookup.LSOA11CD) ";
  string group_by = "GROUP BY lsoa_code ";
  string order_by = "ORDER BY `#crimes` ";
  string h_or_l_str = "";
  if (h_or_l_input == 1)
  {
    h_or_l_str = "DESC ";
  }
  else if (h_or_l_input == 2)
  {
    h_or_l_str = "ASC ";
  }

  string limit = "LIMIT " + to_string(limit_input) + ";";

  string h_or_l_crime_lsoa_query = select + from + group_by + order_by + h_or_l_str + limit;

  cout << h_or_l_crime_lsoa_query << endl;

  if (h_or_l_input == 1)
  {
    cout << "The top " + to_string(limit_input) + " LSOA's with the highest crime are:" << endl;
    ;
  }
  else if (h_or_l_input == 2)
  {
    cout << "The top " + to_string(limit_input) + " LSOA's with the lowest crime are:" << endl;
  }

  query(pMysql, h_or_l_crime_lsoa_query, false);

  return 0;
}

int top_crime_types_in_lsoa(MYSQL *pMysql)
{
  string lsoa_input = "";
  cout << "Which LSOA are you interested in?" << endl;
  lsoa_input = get_str_input();

  int limit_input;
  cout << "How many entries do you want to see?" << endl;
  limit_input = get_int_input();

  string select = "SELECT major_category, COUNT(major_category) AS `#occurences` ";
  string from = "FROM london_crime_by_lsoa ";
  string where = "WHERE lsoa_code = \"" + lsoa_input + "\"" + " ";
  string group_by = "GROUP BY major_category ";
  string order_by = "ORDER BY `#occurences` DESC ";
  string limit = "LIMIT " + to_string(limit_input) + ";";

  string top_crime_types_in_lsoa_query = select + from + where + group_by + order_by + limit;

  cout << top_crime_types_in_lsoa_query << endl;

  cout << "The top " + to_string(limit_input) + " crime types in LSOA: " + lsoa_input + " are:" << endl;

  query(pMysql, top_crime_types_in_lsoa_query, false);

  return 0;
}

// Client Method
void client()
{

  cout << "Welcome to the London Crime Client!" << endl;

  bool terminate = false;

  const char *USERID = "t48shen"; // Your UserID here
  const char *SERVER = "marmoset04.shoshin.uwaterloo.ca";
  const char *DBNAME = "db356_t48shen"; // Can specify a database; don't want to

  MYSQL mysql;            // Space for handler
  MYSQL *pMysql = &mysql; // Pointer to handler

  if (!mysql_init(pMysql))
  {
    cerr << ": mysql_init() error: insufficient memory" << endl
         << "Terminating" << endl;
    terminate = true;
    return;
  }

  // Prompt for a password and connect (use default port 3306)
  const char *passwd = getpass("Password: ");
  if (!mysql_real_connect(pMysql,
                          SERVER,
                          USERID,
                          passwd,
                          DBNAME,
                          0,    // Use default port (3306)
                          NULL, // Not using unix socket or named pipe
                          0))
  { // No client flags
    cerr << ": mysql_real_connect() error: " << mysql_error(pMysql) << endl
         << "Terminating" << endl;
    terminate = true;
    return;
  }
  // remember to mysql_close(pMysql); at the end of program

  while (!terminate)
  {

    // 1. Ask whether the user is a Police Officer or a Citizen.

    bool is_official = false;

    cout << "Enter 1 if you are a policing official." << endl;
    cout << "Enter 2 if you are a citizen." << endl;
    cout << "Enter 0 to exit client." << endl;

    int user_input = get_int_input();

    if (user_input == 1)
    {
      is_official = true;
    }
    else if (user_input == 2)
    {
      is_official = false;
    }
    else if (user_input == 0)
    {
      terminate = true;
      continue;
    }
    else
    {
      // invalid input
    }

    // 2. A menu with enumeration actions associated to each verb
    // Actions include: Add, Update, Search

    if (is_official == true)
    {

      cout << "What do you want to do?." << endl;
      cout << "Enter 1 if you want to add a crime." << endl;
      cout << "Enter 2 if you want to update a crime." << endl;
      cout << "Enter 3 if you want to search for a crime." << endl;
      cout << "Enter 4 if you want to list Top crime types in a certain LSOA." << endl;
      cout << "Enter 5 if you want to see the LSOA's with the HIGHEST/LOWEST crime." << endl;
      cout << "Enter 0 to exit client." << endl;

      int input = get_int_input();

      if (input == 1)
      {
        // add
        if (add_entry(pMysql) != 0)
        {
          cerr << "Add entry failed..." << endl;
          terminate = true;
        }
      }
      else if (input == 2)
      {
        // update
        if (update_entry(pMysql) != 0)
        {
          cerr << "Update entry failed..." << endl;
          terminate = true;
        }
      }
      else if (input == 3)
      {
        // search
        if (search_entry(pMysql) != 0)
        {
          cerr << "Search entry failed..." << endl;
          terminate = true;
        }
      }
      else if (input == 0)
      {
        cerr << "Exiting..." << endl;
        terminate = true;
        continue;
      }
      else if (input == 4)
      {
        if (top_crime_types_in_lsoa(pMysql) != 0)
        {
          cerr << "Query failed..." << endl;
          terminate = true;
        }
      }
      else if (input == 5)
      {
        if (h_or_l_crime_lsoa(pMysql) != 0)
        {
          cerr << "Query failed..." << endl;
          terminate = true;
        }
      }
    }
    else
    {
      // User is a citizen. No update or add crime.
      cout << "What do you want to do?." << endl;
      cout << "Enter 1 if you want to search for a crime." << endl;
      cout << "Enter 2 if you want to list top crime types in a certain LSOA." << endl;
      cout << "Enter 3 if you want to see the top LSOA's with the HIGHEST/LOWEST crime." << endl;

      int input = get_int_input();

      if (input == 1)
      {
        int result = search_entry(pMysql);
        if (result == -2)
        {
          cerr << "Exiting..." << endl;
          terminate = true;
        }
        else if (result == -1)
        {
          cerr << "Search entry failed..." << endl;
          terminate = true;
        }
      }
      else if (input == 2)
      {
        if (top_crime_types_in_lsoa(pMysql) != 0)
        {
          cerr << "Query failed..." << endl;
          terminate = true;
        }
      }
      else if (input == 3)
      {
        if (h_or_l_crime_lsoa(pMysql) != 0)
        {
          cerr << "Query failed..." << endl;
          terminate = true;
        }
      }
    }

    //close
    mysql_close(pMysql);
  }
}

// Main Method
int main(const int argc, const char *argv[])
{
  client();
  return 0;
}
