# simple-compiler
A simple compiler made with c++ and intel machine code for a semester long project.

Supports data types of 4-byte integers, booleans, and string constants. Added tokenizing for floating numbers in code, but not built-in to compiler syntax.
Supports for and while loops. Supports if, else, else if keywords with logical and relational expression evaluation. Similar to c++ syntax.
Built-in read function, which takes in an 4-byte integer.
Built-in print function, which takes arguments to be printing to standard output in comma seperated fields.
Incorporates slight error checking. Not fully developed.

Similar to c++ syntax except:
 data type int4
 comments #, <<- ->> (multiline)
 := for assign operation
 ...
 
Displays size of the bytes needed to execute the program
Example program below:

<<-
  This program computes the date of Easter for a given year.

  Algorithm from "Practical Astronomy with Your Calculator", 2e
  Peter Duffett-Smith, 1981

->>

int4 a;
int4 b;
int4 c;
int4 d;
int4 e;
int4 f;
int4 g;
int4 h;
int4 i;
int4 k;
int4 l;
int4 m;
int4 n;
int4 p;

int4 year;

print("This program calculates the date of Easter for the Gregorian calendar\n");
print("(from 1583 on).\n\n");

print("Please enter a year (< 0 to quit): ");
read(year);

while( year >= 0 )
{
  if(year <= 1582)
    print("Inavlid year.  Must be greater than 1582.\n");
  else
    {
      a := year mod 19;
      b := year / 100;   c := year mod 100;
      d := b / 4;        e := b mod 4;
      f := (b+8) / 25;
      g := (b - f + 1) / 3;
      h := (19 * a + b - d - g + 15) mod 30;
      i := c / 4;        k := c mod 4;
      l := (32 + 2*e + 2*i - h - k) mod 7;
      m := (a + 11*h + 22*l) / 451;
      n := (h + l - 7*m + 114)/31;
      p := (h + l - 7*m + 114) mod 31;

      print("Easter Sunday ", year, " is ", p+1);
      if(n = 3)
        print(" March.\n\n");
      else if (n = 4)
        print(" April.\n\n");
      else
        print("Inavlid month: ", m, "\n");
      
    }
	
  print("Please enter a year (< 0 to quit): ");
  read(year);

}

print("Exiting...\n");
