
class Parser
{
public:
    Parser(std::string &fileName)
    {
        readFromFile.open(fileName, readFromFile.in);
    };

    enum CommandType
    {
        A_COMMAND,
        C_COMMAND,
        L_COMMAND
    };

    bool hasMoreCommands()
    {
        return !readFromFile.eof();
    }

    void advance()
    {
        if (!hasMoreCommands())
            return;

        std::getline(readFromFile, current_command, '\n');
        setCommantType();
    };

    bool isNumber(std::string str)
    {
        for (int i = 1; i < str.length(); i++)
            if (isdigit(str[i]) == false)
                return false;
        return true;
    }

    void setCommantType()
    {
        bool uses_reference_char = current_command[0] == '@';
        bool defines_variable = current_command[0] == '(' && current_command[(current_command.size() - 1)] == ')';

        if (!uses_reference_char && !defines_variable)
        {
            current_command_type = CommandType::C_COMMAND;
            return;
        }

        if (!defines_variable && uses_reference_char && isNumber(current_command))
        {
            current_command_type = CommandType::A_COMMAND;
            return;
        }

        current_command_type = CommandType::L_COMMAND;
        return;
    }

    CommandType commandType()
    {
        return current_command_type;
    };

    std::string symbol()
    {
        if (current_command_type == CommandType::C_COMMAND)
            return "NULL";

        if (current_command_type == CommandType::L_COMMAND)
        {
            return current_command.substr(1, current_command.length() - 2);
        }

        if (current_command_type == CommandType::A_COMMAND)
        {
            // if the table contains the sybmol return it's pointer
            // if the symbol is a number return its binary form
            // convert the string to an int
            // convert the int to it's binary form string
            // couple of ways to do this. Read the string from right to left and do bitshifts, use a library

            // could be @R1, @someSymbol, @Rsomesymbol <- handle all
            int start = current_command.find('R');
            if (start == std::string::npos)
                start = 1;
            else
                start = 2;
            // should test the bound
            std::string symbol = current_command.substr(start, current_command.length() - 1);
            int int_symbol = std::stoi(symbol); // what happens if it isn't an int??
            return convertIntToStringBits(int_symbol);
        }

        return "NULL";
    };

    std::string convertIntToStringBits(int int_symbol)
    {
        // do the loop to build the result
        std::string bit_form = "";
        while (int_symbol != 0)
        {
            int remainder = int_symbol % 2;
            if (remainder != 0)
            {
                bit_form = "1" + bit_form;
                int_symbol--;
            }
            else
                bit_form = "0" + bit_form;
            int_symbol /= 2;
        }
        return bit_form;
    }

    void padWithZeros(std::string &str, std::string &instruction)
    {
        size_t n = 16;
        int precision = n - std::min(n, str.size());
        instruction = std::string(precision, '0').append(str);
    }

    std::string dest()
    {
        // dest=comp;jump
        if (current_command_type != CommandType::C_COMMAND)
            return NULL_STRING;
        // loop the line from beggining to = and construct the dest clause
        int equals_pos = current_command.find("=");
        int semicolon_pos = current_command.find(";");

        if (equals_pos == std::string::npos)
            return NULL_STRING;

        int end;
        if (semicolon_pos != std::string::npos)
            end = semicolon_pos;
        else
            end = 1;

        return constructResultString(0, end, false);
    };

    std::string comp()
    {
        if (current_command_type != CommandType::C_COMMAND)
            return NULL_STRING;

        // loop the line from beggining to = and construct the dest clause
        int equals_pos = current_command.find("=");
        int semicolon_pos = current_command.find(";");

        int start;
        if (equals_pos != std::string::npos)
            start = equals_pos + 1;
        else
            start = 0;

        int end;
        // if no ; comp goes till end
        if (semicolon_pos != std::string::npos)
            end = semicolon_pos;
        else
            end = current_command.length();

        return constructResultString(start, end, false);
    };

    std::string constructResultString(int start, int end, bool checkingJmpCommand) {
        std::string result_string = "";

         for (int i = start; i < end; i++)
        {
            char current_char = current_command[i];
            if (!checkingJmpCommand && current_char == ';' || current_char == '=')
                break;

            if (current_char == ' ')
                continue;

            result_string += current_char;
        }

        return result_string;
    }

    std::string jump()
    {
        if (current_command_type != CommandType::C_COMMAND)
            return NULL_STRING;

        int semicolon_pos = current_command.find(";");

        int start;
        if (semicolon_pos != std::string::npos)
            start = semicolon_pos + 1;
        else
            return NULL_STRING;

        return constructResultString(start, current_command.length(), true);
    };

    void backToTop()
    {
        readFromFile.clear();
        readFromFile.seekg(0);
    }

    void closeFile()
    {
        readFromFile.close();
    }

    // cleanup
    ~Parser()
    {
        readFromFile.close();
    };

private:
    std::string NULL_STRING = "NULL";
    std::string current_command;
    std::fstream readFromFile;
    CommandType current_command_type;
};
