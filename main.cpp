// Armando Lopez CPSC 323 Section 3 LEXER Assignment 3 Friday Iteration

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;


struct tokenInfo
{
	string token; // token for current string
	string lexeme; // lexeme type for current string (*NOTE, initially this will hold an entire line (till a space) till one lexeme is found)
						// the rest will be placed into rest of text
	string restOfText; // remainder of the text still need to be parsed
	bool endReached = false; // end of string
	bool commentBlockEnd = true; // end of comment block (if applicable), used to ignore text till next ! is found. if false , ignore all text
																								// till second ! is found
};

tokenInfo lexer(tokenInfo s, string kW[], char sep[], char opp[])
{

	if (s.lexeme == "" || s.lexeme == " " || s.lexeme == "!") //if the lexeme is a space, null, or !, end this function
	{
		s.lexeme = " ";
		s.restOfText = " ";
		return s;
	}

	static int max = 25; // max number of characters the char array can be
	char c[25];

	for (int i = 0; i < max; i++) // fill the character array with spaces to avoid issues (reading a null
	{								// character from the array)
		c[i] = ' '; 
	}



	strcpy_s(c, s.lexeme.c_str()); // place the current lexeme into the character array for parsing

	for (int i = 0; i < 18; i++)
	{
		if (s.lexeme == kW[i])
		{
			s.token = "KeyWord";
			return s;
		}
	}

	for (int i = 0; i < 13; i++)
	{
		if (c[0] == sep[i])
		{
			s.token = "Seperator";
			return s;
		}
	}

	for (int i = 0; i < 8; i++)
	{
		if (c[0] == opp[i])
		{
			s.token = "Operator";

			return s;
		}
	}


	string tex = ""; // current text in an identifier or number. This saves up until the end of the id/num
						// which allows us to remove only this lexeme, incase no space was placed
	// Case: identifier
	// check initial character

	// State 1: search for first character ------------------------------------------------------------------
	 if ((c[0] >= 'a' && c[0] <= 'z') || (c[0] >= 'A' && c[0] <= 'Z'))   // State 2: In identifier
	{
		tex = tex + c[0];

		for (int i = 1; i < s.lexeme.size(); i++) // verify each character after is acceptable
		{
			if ((c[i] >= 'a' && c[i] <= 'z') || (c[i] >= 'A' && c[i] <= 'Z') ||    // state 2: In identifier (letter or & or number)
				(c[i] >= '0' && c[i] <= '9') || c[i] == '&')
			{
				tex = tex + c[i];
			}
			else
			{
				s.token = "Invalid Identifier";
				s.lexeme = s.restOfText;
			}
		}

		s.lexeme = tex;	
		s.token = "Identifier";	// state 3: End State, end of Identifier
	}


	// case number / real number


	else if ((c[0] >= '0' && c[0] <= '9')) // State 4: Start of digit (whole)
	{
		tex = tex + c[0];
		int i = 1;
		s.token = "Integer(whole)"; // if . not found, State 5: FINAL STATE: whole number
		while (i < s.lexeme.size())
		{
			if (c[i] >= '0' && c[i] <= '9') // State 4: In Whole Intenger
			{
				tex = tex + c[i];
			}
			else if (c[i] == '.') // State 6: in Floating Integer (can not revert back to state 4.)
			{
				tex = tex + c[i];
				s.token = "Integer(float)"; // if . found, eventually .....STATE 7: FINAL STATE: floating number
			}
			i++;
		}
		s.lexeme = tex;
	}
	




	return s;
}

tokenInfo spacingCheck(tokenInfo s)
{



	static int arraySize = 200; // array size (change in array too) -----*****
	// need to check incase of bad spacing e.g if(---) instead of if (---)
	char textChar[200]; // will hold the chunk of text (if applicable)

	int size = s.lexeme.size();


	strcpy_s(textChar, s.lexeme.c_str()); // text is in char array for easier character parsing

	for (int i = s.lexeme.size(); i < arraySize; i++) // initialize every array space as a space for parsing loop purposes
	{
		textChar[i] = ' ';
	}


	for (int i = 0; i < s.lexeme.size() || s.endReached == true; i++)
	{

		if (textChar[i] == ' ') // skip spacing
		{
			s.endReached = true;
			return s;
		}



		//            THIS CHUNK OF CODE WILL FIND COMMENT BLOCKS IN A GIVEN TEXT STRING    ------------------------
		else if (s.commentBlockEnd == false || textChar[i] == '!') // if text comment block start is detected, 
		{																//OR the second comment block has not been found


			if (textChar[i] == '!' && s.commentBlockEnd == true) // if it is the first !
			{
				s.commentBlockEnd = false; // there is no second !, keep parsing till found

			}
			else if (textChar[i] == '!' && s.commentBlockEnd == false) // if second !
			{
				s.commentBlockEnd = true; // second ! found, finish comment and continue as normal


			}
			else if (textChar[i] != '!' && s.commentBlockEnd == false) // if space after first ! but before second !
			{

				s.commentBlockEnd = false; // there is no second !, keep parsing till found. This prevents
											// identifiers from being "found" while in comments
			}

		}
		// END OF COMMENT BLOCK CHECK ----------------------------
		// continue parsing through the rest of the char array

		// will check for separator

		else if (textChar[i] == '\'' || textChar[i] == '(' || textChar[i] == ')' || textChar[i] == '{'		// If in separator array
			|| textChar[i] == '}' || textChar[i] == '[' || textChar[i] == ']' || textChar[i] == ','
			|| textChar[i] == '.' || textChar[i] == ':' || textChar[i] == ';' || textChar[i] == '!' || textChar[i] == ' ')
		{
			s.lexeme = textChar[i]; // lexeme is the first character

			if (i == s.lexeme.size()) // if the cap has been reached, this line is done, move onto next line
			{
				s.endReached = true;
			}
			i++;
		}

		else if (textChar[i] == '*' || textChar[i] == '+' || textChar[i] == '-' || textChar[i] == '=' // if in operator array
			|| textChar[i] == '/' || textChar[i] == '<' || textChar[i] == '>' || textChar[i] == '%')
		{

			s.lexeme = textChar[i]; // lexeme is first char

			if (i == s.lexeme.size()) //if the cap has been reached, this line is done, move onto next line
			{
				s.endReached = true;
			}

			i++;

		}

		else
		{ // will check for identifiers, keywords, or numbers

			s.lexeme = "";
			while ((textChar[i] >= 'a' && textChar[i] <= 'z') || (textChar[i] >= 'A' && textChar[i] <= 'Z') ||
				(textChar[i] >= '0' && textChar[i] <= '9') || textChar[i] == '&' || textChar[i] == '.') // while char is numb/letter/&/.
			{
				s.lexeme = s.lexeme + textChar[i]; // add char to s.lexeme (string)

				if (i == s.lexeme.size()) // if the end of the array has been reached
				{
					s.endReached = true;
				}
				i++;
			}

		}


		if (s.endReached == false) // if the end of the string has not been reached (i.e   if(x) would be four tokens in the same string)
		{
			s.restOfText = "";

			if (i == size) // if the character size is = to initial lexeme string size (a.k.a all characters were parsed through)
			{
				s.restOfText = ""; 
				s.endReached = true; // end of the current string line read.
			}
	

			while (i < size ) // place the remainder of the string into the array
			{
				s.restOfText = s.restOfText + textChar[i];
				//cout << "test end reach statement " << s.restOfText << "end" << endl;
				i++;


			}
		}
	}

	return s;
}


int main()
{
	// create new array in loop


	tokenInfo t; //token Info struct T
	string token; // will hold the token type for the current string
	char lex[10]; // will hold the full lexeme string (in characters) to print out

	// arrays for keywords, separators, operators
	string keyWords[18] = { "int","float","bool","if","else","then","endif","while","whileend","do","doend","for","forend","input","output","and","or","function" };
	char separators[13] = { '\'','(',')','{','}','[',']',',','.',':',';','!',' ' };
	char operators[8] = { '*','+','-','=','/','<','>','%' };
	// identifiers,comments,numbers/realnumbers will be checked in a case by case basis



	ifstream myfile;

	myfile.open("sample.txt"); // read from file

	ofstream myFileInput;
	myFileInput.open("output.txt"); // read into file

	myFileInput << setw(30) << left << "Token" << right << "Lexeme" << endl;   // table column creation
	myFileInput << "# # # # # # # # # # # # # # # # # # # # # # # # # # # # #" << endl; // separation (for a nicer look)

	while (!myfile.eof()) // while the file has not come to an end
	{
		myfile >> t.lexeme; // read in the next line (up until a space) into the token's lexeme
		t.restOfText = t.lexeme;
		t.endReached = false; // has the end of the passed in lexeme been read in? 
		t.token = ""; // token is currently blank

		while (t.endReached == false) // has the end of the previous string been reached?
		{
			t = spacingCheck(t);
			t = lexer(t, keyWords, separators, operators);


			if (t.lexeme != " " && t.commentBlockEnd == true) // if lexeme is not blank, and not within comments
			{
				myFileInput << setw(30) << left << t.token << right << t.lexeme << endl;
			}
			t.lexeme = t.restOfText; // place rest of the string line to continue to be parsed

			if (t.lexeme == " ") // if the lexeme is empty, then the end of this current string line has been reached
			{
				t.endReached = true;
			}
		}
	}
	cout << "Generating list of tokens for lexor.";
	cout << "Text File created. Please look for \"output.txt\" file." << endl;
	myFileInput.close(); // close output file
	myfile.close(); // close sample file
	//----------------------------------------------------------------------------------
	// Syntax Analysis Portion, Assignment 2
	cout << "Syntax Analysis:" << endl << endl;

	myfile.open("output.txt"); // read from output file
	myFileInput.open("test.txt");

	string text;

	while (!myfile.eof()) // while file has not come to an end
	{
		myfile >> text;

		if (text != "#" && text != "Token" && text != "Lexeme")
		{
			myFileInput << text << endl;
		}
	}

	myFileInput.close();
	myfile.close();




	//table

	char tableArray[5][6] =
	{
		('b','|','|','|','|','|'),
		('|','c','|','|','|','|'),
		('d','|','e','d','|','|'),
		('|','b','e','|','|','|'),
		('|','|','|','|','|','$'),
	};

	char state = 'a'; // intitial state to begin running
	bool equalsBefore = false; // there was an "=" before the identifier/number
	bool firstToken = true; // first token in the file

	string tokText; // holds the current token text
	string opText; // hold the current lexeme text


	myfile.open("test.txt"); // read from output file
	myFileInput.open("syntaxOutput.txt");


	while (!myfile.eof()) // while file has not come to an end
	{
		myfile >> tokText;
		myfile >> opText;

		// ASSIGNMENT OPERATORS SOLUTION
		if(tokText == "Identifier" || tokText == "Operator" || tokText == "Seperator" || tokText == "Integer(whole)" || tokText == "Integer(float)")
		{
			myFileInput << "Token: " << tokText << "            " << opText << endl;


			// state 1

			if (state == 'a' && opText != ";")
			{
				if (tokText == "Identifier")
				{

					if (firstToken == true)
					{
						myFileInput << "<Statement List> -> <Statement> | <Statement> <Statement List>" << endl;
						firstToken = false;
					}


					state = 'b';
					myFileInput << "<Statement> -> <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While> " << endl;
					myFileInput << "<Assign> -> <Identifier> = <Expression>;" << endl;
				}


				else {
					myFileInput << "Invalid Syntax. Closing Program";
					exit(0);
				}


			}

			// state 2
			else if (state == 'b')
			{
				stateb:

				if (tokText == "Operator")
			{
				if (opText == "=")
				{
					equalsBefore = true;
				}
				else if (opText == "+" || opText == "-" || opText == "*" || opText == "/")
				{
					myFileInput << "<Empty>->Epsilon" << endl;
					myFileInput << "<TermPrime> -> * <Factor> <TermPrime> | / <Factor> <TermPrime> | <Empty>" << endl;
					if (opText == "+" || opText == "-")
					{
						myFileInput << "<Empty>->Epsilon" << endl;
						myFileInput << "<ExpressionPrime> -> + <Term> <ExpressionPrime> | -<Term> <ExpressionPrime> | <Empty>" << endl;
					}
					equalsBefore = false;
				}
				else
				{
					myFileInput << "Invalid Syntax. Closing Program";
					exit(0);
				}
				state = 'c';
			}

				else
				{
					myFileInput << "Invalid Syntax. Closing Program";
					exit(0);
				}


			}

			// state 3
			else if (state == 'c')
			{
				if (tokText == "Identifier"|| tokText == "Integer(whole)" || tokText == "Integer(float)")
				{

					if (equalsBefore == true)
					{
						myFileInput << "<Expression> -> <Term> <ExpressionPrime>" << endl;
					}
					
					myFileInput << "<Term> -> <Factor> <TermPrime>" << endl;
					myFileInput << "<Factor> -> - <Primary> | <Primary>" << endl;
					myFileInput << "<Primary> -> <Identifier> | <Integer> | <Identifier> (<IDs>) | (<Expression>) | <Real> | true | false" << endl;
					

					state = 'd';
				}

				else
				{
					myFileInput << "Invalid Syntax. Closing Program";
					exit(0);
				}

			}




			// state 4

			else if (state == 'd')
			{
				if (tokText == "Operator")
				{

					goto stateb;
					
				}

				else if (tokText == "Seperator")
				{
					goto stated;

				}


				else
				{
					myFileInput << "Invalid Syntax. Closing Program";
					exit(0);
				}


			}

			// state 5
				else if (state == 'e' || opText == ";")
				{
				stated:
				if (tokText == "Seperator")
				{
					if (opText == ";")
					{
						myFileInput << "<Empty>->Epsilon" << endl;
						myFileInput << "<TermPrime> -> * <Factor> <TermPrime> | / <Factor> <TermPrime> | <Empty>" << endl;
						myFileInput << "<Empty>->Epsilon" << endl;
						myFileInput << "<ExpressionPrime> -> + <Term> <ExpressionPrime> | -<Term> <ExpressionPrime> | <Empty>" << endl;
						myFileInput << "<Empty>->Epsilon" << endl;

						state = 'a'; // end statement , restart for next statement (if applicable)
					}
				}

				else
				{

					myFileInput << "Invalid Syntax. Closing Program";
					exit(0);
				}
				}


		}


		else if (tokText == "KeyWord") // DECLARATION / WHILE LOOP / IFS
		{

		if (opText == "while")
		{
			myFileInput << "Token: " << tokText << "            " << opText << endl;
			myFileInput << "<While>:: = while ( <condition> ) <Statement> whileend" << endl;
			myfile >> tokText;
			myfile >> opText;
			
			if (tokText == "Seperator" && opText == "(")
			{
				myFileInput << "Token: " << tokText << "            " << opText << endl;
				myfile >> tokText;
				myfile >> opText;

				if (tokText == "Seperator" && opText == ")")
				{
					myFileInput << "Token: " << tokText << "            " << opText << endl;
					myfile >> tokText;
					myfile >> opText;

					if (tokText == "KeyWord" && opText == "whileend")
					{
						myFileInput << "Token: " << tokText << "            " << opText << endl;

					}
					else
					{
						myFileInput << "Error: Expected whileend" << endl;
					}
				}
				else
				{
					myFileInput << "Error: Expected (" << endl;
				}
			}
			else
			{
				myFileInput << "Error: Expected (" << endl;
			}


		}

	/*	else if (tokText == "if")
		{

			myFileInput << "<If>:: = if <Conditional> then <Statement> else <Statement> endif" << endl;
			myfile >> tokText;
			myfile >> opText;
			if (tokText == "Seperator" && opText == "(")
			{
				myFileInput << "Token: " << tokText << "            " << opText << endl;


				myfile >> tokText;
				myfile >> opText;
				if (tokText == "Seperator" && opText == ")")
				{
					myFileInput << "Token: " << tokText << "            " << opText << endl;
				}
				else
				{
					myFileInput << "Error: Expected (" << endl;
				}

				

			}
			else
			{
				myFileInput << "Error: Expected (" << endl;



		}

		

		*/


		} 
		
		
		
		
		
		
		
		



	}

	myFileInput.close();
	myfile.close();






	system("pause");
	return 0;
}