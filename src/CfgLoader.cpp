/*
     This file is part of RaspberryPICANSlave
     (C) Riccardo Ventrella 2017
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 3.0 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <fstream>
#include <algorithm>
#include <string>
#include <CfgLoader.h>
#include <Slaves.h>


using namespace std;

// Uncomment this to DUMP verbosely debug stuff on stdout
//#define DUMP

// Cfg var entry names
#define VAR_CTRL_ID       "CTRL_ID"
#define VAR_EXPIRE_TS     "EXPIRE_TS"



// This f() strips the given string by leading/trailing spaces
static inline string &Strip(string &s)
{
   // Strip from the start
   s.erase(s.begin(), find_if(s.begin(), s.end(), [](int c) {return !isspace(c); }));
   // Strip from the end
   s.erase(find_if(s.rbegin(), s.rend(), [](int c) {return !isspace(c); }).base(), s.end());
   return s;
}

// This f() takes the given string, assuming it's representing a dec number, and return its relative int
static inline int FromDec(const string &s) { return strtoul(s.c_str(), NULL, 10); }

// This f() takes the given string, assuming it's representing a hex number, and return its relative int
static inline int FromHex(const string &s) { return strtoul(s.c_str(), NULL, 16); }

// This f() turn a string into its upper format
static inline void ToUpper(string& str) { for (auto & c : str) c = toupper(c); }


// This f() checks if the given Line line is a section one, i.e. is made by a "[ ... ]" statement
// If a section matching is found, the f() extract its content as a hex number, returning it.
static int GetSection(const string Line)
{
   string SubString;

   size_t Found = Line.find_first_of("[");
   if (Found == std::string::npos)
      return 0;
   SubString = Line.substr(Found+1);
   // Find the ending "]" and force the substring here
   Found = SubString.find_first_of("]");
   if (Found == std::string::npos)
      return 0;
   SubString.resize(Found);
#ifdef DUMP
   printf("Found Section: %s\n", SubString.c_str());
#endif   
   // Strip this substring, to get a bare version of it
   Strip(SubString);
   // Nothing to do if the line became empty
   if (SubString.empty())
      return 0;
   // Extract its hex number
   return FromHex(SubString);
}

// This f() parses the given line trying to extract the VarName + Value from it.
// We are assuming this is not a section line in advance, in order to avoid useless tests.
static bool GetVarValuePair(string& VarName, int& Value, const string& Line)
{
   // Search for a "=" instance within the Line string
   size_t Found = Line.find_first_of("=");
   if (Found == std::string::npos)
      // Not a valid Var + Value line so far
      return 0;
   // Extract the value substring
   string ValueString;
   ValueString = Line.substr(Found+1);
   Strip(ValueString);
   // Trim the Line string to the variable one
   VarName = Line;
   VarName.resize(Found);
   // Always work with an uppercase var name string
   ToUpper(VarName);
   Strip(VarName);
#ifdef DUMP
   printf("Var = %s, Value = %s\n", VarName.c_str(), ValueString.c_str());
#endif
   // Get the number from the ValueString. If the string contains a "x", we assume it's in hex format.
   Found = ValueString.find_first_of("x");
   if (Found != std::string::npos)
      Value = FromHex(ValueString);
   else
      Value = FromDec(ValueString);

   return true;
}

// This f() parses the given Cfg file line by line, loading it in the global shared slaves table.
// If a Cfg has not been found, simply leave the Slaves as empty: they will be filled up runtime
// on each CAN Status received message. In this case, all expire thresholds will be defaulted by
// the define on the beginning of this page.
void LoadCfgFile(const char CfgFilePath[])
{
   ifstream CfgFile(CfgFilePath);
   // Test the file has been opened or fail instead
   if (!CfgFile)
   {
#ifdef DUMP
	   printf("Cfg not found!!!");
#endif	  
      return;
   }

   string Line, VarName;
   int    SectionNumber, Value;

   // Parse the file line by line
   while (getline(CfgFile, Line))
   {
      // Nothing to do if the line became empty
      if (Line.empty())
         continue;
      // Remove the comments, i.e. all the substring after a possible ";"
      size_t Found = Line.find_first_of(";");
      if (Found != std::string::npos)
         // Shrink the string removing the comment
         Line.resize(Found);
      // Strip the line leading/trailing spaces
      Strip(Line);
      // Nothing to do if the line became empty
      if (Line.empty())
         continue;
#ifdef DUMP
	  printf("Line: <%s>\n", Line.c_str());
#endif
      // Check this line is a section, returnin its embedded hex number, eventually
      SectionNumber = GetSection(Line);
	  if(SectionNumber)
		  Slave_AddID(SectionNumber);
      else
      {
         // Here it comes a "variable = value" pair. Get this couple and update current Slave, if any.
         // If the line is not found to be a valid var + value pair, or it there's not any valid Slave
         // let's simply ignore this line, skipping to the next one.
		 if(!SlavesAreEmpty())
         {
            // Parse the current line and retrieve its var + value pair, before appending it
            if (GetVarValuePair(VarName, Value, Line))
            {
               // Update its entry just read from the cfg.
               // We have to switch across the VarName to match the proper Slave entry
               if (VarName == VAR_CTRL_ID)
				   Slave_Add_CTRL_ID_ToLastID(Value);
               else if (VarName == VAR_EXPIRE_TS)
				   Slave_Add_ExpireTS_ToLastID(Value);
            }
         }
      }
   }
#ifdef DUMP
	Slave_DUMPSlavesForDebug();
#endif
}