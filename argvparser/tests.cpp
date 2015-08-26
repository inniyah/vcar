#include <iostream>
#include "argvparser.h"

using namespace std;
using namespace CommandLineProcessing;

bool ok()
{
    cout << "OK" << endl;
    return(true);
}

bool failed()
{
    cout << "Failed" << endl;
    return(false);
}

bool testHelpOptionDetection()
{
    cout << "Detect help option -> ";

    int argc = 2;
    char* argv[] = {"app","-h"};

    ArgvParser parser;
    parser.setHelpOption("help", "h", "help descr.");
    ArgvParser::ParserResults results = parser.parse(argc, argv);
    if (results != ArgvParser::ParserHelpRequested)
    {
        cout << "help: " << (results==ArgvParser::ParserHelpRequested) << endl;
        cout << "unknown: " << (results==ArgvParser::ParserUnknownOption)  << endl;
        cout << "optionafter: " << (results==ArgvParser::ParserOptionAfterArgument)  << endl;
        cout << "missingvalue: " << (results==ArgvParser::ParserMissingValue)  << endl;
        cout << "muliple: " << (results==ArgvParser::ParserMalformedMultipleShortOption)  << endl;
        cout << "required: " << (results==ArgvParser::ParserRequiredOptionMissing)  << endl;
        return(failed());
    }
    return(ok());
}

bool testUnknownOptionDetection()
{
    cout << "Detect unknown options -> ";

    int argc = 8;
    char* argv[] = { "app","--first_long","value1","-fdev","--second_long","value2","arg1","arg2"};

    ArgvParser parser;

    if (parser.parse(argc, argv) != ArgvParser::ParserUnknownOption)
        return(failed());

    return(ok());
}

bool testForbiddenOptionPosition()
{
    cout << "Detect forbidden option pos -> ";

    int argc = 8;
    char* argv[] = { "app","--first_long","value1","-fdev","--second_long","value2","arg1","arg2"};

    ArgvParser parser;
    parser.defineOption("first_long");
    parser.defineOption("f");
    parser.defineOption("d");
    parser.defineOption("e");
    parser.defineOption("v");
    parser.defineOption("second_long", "", ArgvParser::OptionRequiresValue);

    if (parser.parse(argc, argv) != ArgvParser::ParserOptionAfterArgument)
        return(failed());

    return(ok());
}

bool testMissingOptionValue()
{
    cout << "Detect missing values -> ";

    int argc = 7;
    char* argv[] = { "app","--first_long","-fdev","--second_long","value2","arg1","arg2"};

    ArgvParser parser;
    parser.defineOption("first_long", "", ArgvParser::OptionRequiresValue);
    parser.defineOption("f");
    parser.defineOption("d");
    parser.defineOption("e");
    parser.defineOption("v");
    parser.defineOption("second_long", "", ArgvParser::OptionRequiresValue);
    if (parser.parse(argc, argv) != ArgvParser::ParserMissingValue)
        return(failed());

    return(ok());
}

bool testMalformedShortOptions()
{
    cout << "Detect malformed shorts -> ";

    int argc = 8;
    char* argv[] = { "app","--first_long","value","-fdev=5","--second_long","value2","arg1","arg2"};

    ArgvParser parser;
    parser.defineOption("first_long", "", ArgvParser::OptionRequiresValue);
    parser.defineOption("f");
    parser.defineOption("d");
    parser.defineOption("e");
    parser.defineOption("v", "", ArgvParser::OptionRequiresValue);
    parser.defineOption("second_long", "", ArgvParser::OptionRequiresValue);

    if (parser.parse(argc, argv) != ArgvParser::ParserMalformedMultipleShortOption)
    {
        cout <<parser.parse(argc, argv) << endl;
        return(failed());
    }

    return(ok());
}

bool testCorrectParsing()
{
    cout << "Check for correct parsing return value -> ";

    int argc = 12;
    char* argv[] = { "app","--first_long","value1","-fev","--second_long","value2",
                     "-x=5","--third_long=0,1,2","-d","3","arg1","arg2"};

    ArgvParser parser;
    parser.defineOption("first_long", "", ArgvParser::OptionRequiresValue);
    parser.defineOption("f");
    parser.defineOption("xtra", "", ArgvParser::OptionRequiresValue);
    parser.defineOptionAlternative("xtra", "x");
    parser.defineOption("e");
    parser.defineOption("sep", "", ArgvParser::OptionRequiresValue);
    parser.defineOption("v");
    parser.defineOption("d", "", ArgvParser::OptionRequiresValue);
    parser.defineOption("second_long", "", ArgvParser::OptionRequiresValue);
    parser.defineOption("third_long", "", ArgvParser::OptionRequiresValue);

    if (parser.parse(argc, argv) != ArgvParser::NoParserError)
        return(failed());

    if (parser.arguments() != 2
            || parser.argument(0) != "arg1"
            || parser.argument(1) != "arg2"
            || !parser.foundOption("f")
            || !parser.foundOption("d")
            || !parser.foundOption("e")
            || !parser.foundOption("v")
            || parser.foundOption("g")
            || !parser.foundOption("first_long")
            || !parser.foundOption("second_long")
            || parser.optionValue("second_long") != "value2"
            || parser.foundOption("sep")
            || !parser.foundOption("x")
            || !parser.foundOption("xtra")
            || parser.optionValue("xtra") != "5"
            || !parser.foundOption("d")
            || parser.optionValue("d") != "3"
            || !parser.foundOption("third_long")
            || parser.optionValue("third_long") != "0,1,2")
    {
        cout << parser.arguments() << endl;
        return(failed());
    }

    return(ok());
}

bool testRequiredOptions()
{
    cout << "Check for required options -> ";

    int argc = 3;
    char* argv[] = { "app","--first_long","-f"};

    ArgvParser parser;
    parser.defineOption("first_long");
    parser.defineOption("f", "", ArgvParser::OptionRequired);
    parser.defineOption("d", "", ArgvParser::OptionRequired);

    if (parser.parse(argc, argv) != ArgvParser::ParserRequiredOptionMissing)
        return(failed());

    return(ok());
}

bool testOptionalOptionValues()
{
    cout << "Check for optional option values -> ";

    int argc = 3;
    char* argv[] = { "app","--first_long=check","-f"};

    ArgvParser parser;
    parser.defineOption("first_long");
    parser.defineOption("f", "");

    if (parser.parse(argc, argv) != ArgvParser::NoParserError)
        return(failed());
    if (!(parser.optionValue("first_long") == "check"))
        return(failed());

    return(ok());
}

bool testIDStringExpansion()
{
    cout << "Check ID string expansion -> ";

    vector<unsigned int> ids;

    if (!expandRangeStringToUInt("1,2-5,6-4", ids))
        return(failed());

    if (ids.size() != 8
            || ids[0] != 1 || ids[1] != 2 || ids[2] != 3 || ids[3] != 4
            || ids[4] != 5 || ids[5] != 6 || ids[6] != 5 || ids[7] != 4)
        return(failed());

    return(ok());
}

int main(int, char*)
{
    cout << ">>> Running commandline parser tests >>>" << endl;

    if (!testHelpOptionDetection() || !testUnknownOptionDetection()
        || !testForbiddenOptionPosition() || !testMissingOptionValue()
        || !testMalformedShortOptions() || !testCorrectParsing()
        || !testRequiredOptions() || !testOptionalOptionValues()
        || !testIDStringExpansion())
        exit(1);

    // finally print a generated help page
    ArgvParser parser;
    parser.setIntroductoryDescription("This is the intro.");
    parser.setHelpOption();
    parser.defineOption("with-value", "Simple description.",ArgvParser::OptionRequiresValue);
    parser.defineOption("value-required",
                        "Complex description which we want to see split nicely over several lines. Perhaps one more sentence is needed.\n After a newline can come a lot of new words that may even form a new sentence which might be too long for a single line. one two three four five six seven eight nine ten eleven twelve ......\nAndFinallyOneExampleOfANecessaryRudeBreakBecauseOfMissingWhitespaceInTheWholeLine. ",
                        ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
    parser.defineOption("alt1");
    parser.defineOptionAlternative("alt1", "alt2");
    parser.addErrorCode(-1, "Standard");
    parser.addErrorCode(6, "Error");
    cout << endl << "Example help page starts here:" << endl << parser.usageDescription() << endl;

    cout << "<<< Finished commandline parser tests <<<" << endl;

}

