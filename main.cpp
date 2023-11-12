#include <iostream>
#include <string>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
   for(int i = 0; i < argc; ++i) 
   {
      std::cout << azColName[i] << "=" << (argv[i] ? argv[i] : "NULL") << "\n";
   }
   return 0;
}

void print(sqlite3* db, std::string* query)
{
   sqlite3_stmt *stmt;  
   if ((sqlite3_prepare_v2(db, query->c_str(), -1, &stmt, 0)) == SQLITE_OK) 
   {
      int col_num = sqlite3_column_count(stmt); 
      int res{};

      std::cout << "Current database state:\n";

      while (res != SQLITE_DONE && res != SQLITE_ERROR)         
      {
         res = sqlite3_step(stmt); 
         if (res == SQLITE_ROW)
         {
            for (int i = 0; i < col_num; ++i)  
            {
               std::cout << sqlite3_column_text(stmt, i) << " " ;
            }
            std::cout << "\n";
         }   
      }
   }
}

int main(int argc, char** argv) 
{
	sqlite3 *db;
   char *zErrMsg = 0;
   std::string sql;

   int ret = sqlite3_open("test1.db", &db);
   
   if(ret) 
   {
      std::cerr << "Can't open database: "<< sqlite3_errmsg(db) << "\n";
      exit(0);
   } else 
   {
      std::cout << "Opened database successfully\n";
   }

   sql ="CREATE TABLE IF NOT EXISTS ACCOUNT(" 
   "NAME           TEXT     NOT NULL," 
   "AGE            INT      NOT NULL," 
   "USERNAME        TEXT     NOT NULL," 
   "PASSWORT        TEXT     NOT NULL);";

   sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

   if(ret != SQLITE_OK)
   {
      std::cerr << "SQL error: " << zErrMsg << "\n";
      sqlite3_free(zErrMsg);
   } else 
   {
      std::cout << "Table created successfully\n";
   }

   std::string query_select_all = "SELECT * FROM ACCOUNT;";
   print(db, &query_select_all);

   std::cout << "Options:\n-to create a new account 'c': \n-to sign in press 's': \n-to exit press 'e': \n";

   char ch{};
   while(ch != 'e')
   {
      std::cin >> ch;

      switch (ch){
         case 'c':
         {
            std::cout << "Please enter your name:";
            std::string name;
            std::cin >>  name;

            std::cout << "Please enter your age: ";
            int age;
            std::cin >>  age;

            std::cout << "Please enter your username: ";
            std::string username;
            std::cin >>  username;

            std::cout << "Please enter your passwort: ";
            std::string passwort;
            std::cin >>  passwort;

            sql = "INSERT INTO ACCOUNT VALUES('" + name + "','" + std::to_string(age) + "','" + username + 
                  "','" + passwort + "');";
            sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
            print(db, &query_select_all);
            break;
         }
         case 's':
         {
            std::cout << "Please enter your username: ";
            std::string username;
            std::cin >> username;

            std::cout << "Please enter your passwort: ";
            std::string passwort;
            std::cin >> passwort;

            sqlite3_stmt *stmt;
            std::string sql = "SELECT USERNAME FROM ACCOUNT WHERE USERNAME='" + username + "' AND"
                              " PASSWORT='" + passwort + "';";

            sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);
            sqlite3_step(stmt);
            std::cout << sqlite3_column_text(stmt, 0);

            std::cout << ", you have successfully logged in.\n Options:\n-to change passwort press 'k':"
                         " \n-to change username press 'u': \n-to log out press 'o': \n";

            char opt{};
            while(opt != 'o')
            {
               std::cin >> opt;
               switch (opt)
               {
                  case 'k':
                  {
                     std::string new_passwort{};
                     std::cout << "Please enter a new passwort:";
                     std::cin >> new_passwort;

                     std::string sql = "UPDATE ACCOUNT SET PASSWORT='" + new_passwort + "' WHERE"
                                       " USERNAME='" + username + "' AND PASSWORT='" + passwort + "';";
                     passwort = new_passwort;
                     sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
                     print(db, &query_select_all);
                     break;
                  }
                  case 'u':
                  {
                     std::string new_username{};
                     std::cout << "Please enter a new username: ";
                     std::cin >> new_username;

                     std::string sql = "UPDATE ACCOUNT SET USERNAME='" + new_username + "' WHERE"
                                       " USERNAME='" + username + "' AND PASSWORT='" + passwort + "';";
                     username = new_username;
                     sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
                     print(db, &query_select_all);
                     break;
                  }
                  case 'o':
                  {
                     break;
                  }
                  default: break;
               }
            }

            sqlite3_finalize(stmt);
            std::cout << "You have been logged out.\n";
            break;
         }
         case 'e':
         {
            break;
         }
         default: break;
      }
   }
   sqlite3_close(db);
}
