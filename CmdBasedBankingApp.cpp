#include <iostream>
#include "source/BankingInfo.h"
#include "bcrypt/include/bcrypt.h"
#include <fstream>
const char* bankingLogo = R"( 
                                   __    __              __      __                                __        _______                       __        __                     
                                  /  \  /  |            /  |    /  |                              /  |      /       \                     /  |      /  |                    
                                  $$  \ $$ |  ______   _$$ |_   $$/   ______   _______    ______  $$ |      $$$$$$$  |  ______   _______  $$ |   __ $$/  _______    ______  
                                  $$$  \$$ | /      \ / $$   |  /  | /      \ /       \  /      \ $$ |      $$ |__$$ | /      \ /       \ $$ |  /  |/  |/       \  /      \ 
                                  $$$$  $$ | $$$$$$  |$$$$$$/   $$ |/$$$$$$  |$$$$$$$  | $$$$$$  |$$ |      $$    $$<  $$$$$$  |$$$$$$$  |$$ |_/$$/ $$ |$$$$$$$  |/$$$$$$  |
                                  $$ $$ $$ | /    $$ |  $$ | __ $$ |$$ |  $$ |$$ |  $$ | /    $$ |$$ |      $$$$$$$  | /    $$ |$$ |  $$ |$$   $$<  $$ |$$ |  $$ |$$ |  $$ |
                                  $$ |$$$$ |/$$$$$$$ |  $$ |/  |$$ |$$ \__$$ |$$ |  $$ |/$$$$$$$ |$$ |      $$ |__$$ |/$$$$$$$ |$$ |  $$ |$$$$$$  \ $$ |$$ |  $$ |$$ \__$$ |
                                  $$ | $$$ |$$    $$ |  $$  $$/ $$ |$$    $$/ $$ |  $$ |$$    $$ |$$ |      $$    $$/ $$    $$ |$$ |  $$ |$$ | $$  |$$ |$$ |  $$ |$$    $$ |
                                  $$/   $$/  $$$$$$$/    $$$$/  $$/  $$$$$$/  $$/   $$/  $$$$$$$/ $$/       $$$$$$$/   $$$$$$$/ $$/   $$/ $$/   $$/ $$/ $$/   $$/  $$$$$$$ |
                                                                                                                                                                  /  \__$$ |
                                                                                                                                                                  $$    $$/ 
                                                                                                                                                                   $$$$$$/  
)";
enum Options {
    BALANCE = 1,
    WITHDRAW,
    DEPOSIT,
    FEE
};
void writeEvent(std::string msg) {
    std::cout << "[~] -> " + msg << std::endl;
}
bool createAccount() {
    const int ROUNDS = 15;
    std::string name, pass;
    std::cout << "[!] -> Input new username: ";
    std::cin >> name;
    std::cout << "[!] -> Input new password: ";
    std::cin >> pass;
    
    // current only allows for 1 login cred
    std::string nameHash = bcrypt::generateHash(name, ROUNDS);
    std::string passHash = bcrypt::generateHash(pass, ROUNDS + 1);

    std::fstream out{ "totallyNotLogins.txt",  std::fstream::trunc | std::fstream::out };

    if (out.fail() || !out.is_open())
        return false;

    out << nameHash + "\n";
    out << passHash;
    out.close();
    return true;
}
bool handleLogin() {
    std::string name, pass, line;
    std::cout << "[!] -> Input your username: ";
    std::cin >> name;
    std::cout << "[!] -> Input your password: ";
    std::cin >> pass;

    std::fstream in;
    in.open("totallyNotLogins.txt", std::fstream::in);
    if (!in.is_open() || in.fail())
        return false;

    std::string hashes[2];
    int lines = 0;
    while (std::getline(in, line)) {
        hashes[lines] = line;
        lines++;
    }
    return bcrypt::validatePassword(name, hashes[0]) && bcrypt::validatePassword(pass, hashes[1]);
}
void handlePostLogin() {
    BankingInfo userInfo;

    userInfo = userInfo.load(); // load user data, should just set it in the function so I don't have to do userInfo = userInfo but oh well...
    system("cls");
    std::cout << bankingLogo << std::endl;

    writeEvent("Thanks for logging in, what would you like to do today?");
    writeEvent("1. Check balance.");
    writeEvent("2. Withdraw.");
    writeEvent("3. Deposit.");
    writeEvent("4. Pay fees.");

    int chosenOption = 0;
    do {
        std::cin >> chosenOption;
    } while (chosenOption >= 5 || chosenOption <= 0);

    switch (chosenOption) {
    case BALANCE: {
        std::cout << "Your balance is: " << userInfo.getBalance() << std::endl;
        break;
    }
    case WITHDRAW: { 
        /*
        You'd want to verify that they have the money and that the value is positive
        Wouldn't want a remake of the early PayPal exploit that let you input
        A negative value and have it added to your account lol.
        */
        float flWithdraw = 0.0f;
        std::cout << "How much would you like to withdraw?" << std::endl;
        std::cin >> flWithdraw;
        userInfo.doWithdraw(abs(flWithdraw)); // abs value to avoid negatives
        std::cout << "Your balance is now: " << userInfo.getBalance() << std::endl;
        break;
    }
    case DEPOSIT: {
        float flDeposit = 0.0f;
        std::cout << "How much would you like to deposit?" << std::endl;
        std::cin >> flDeposit;
        userInfo.doDeposit(abs(flDeposit));// abs value to avoid negatives
        std::cout << "Your balance is now: " << userInfo.getBalance() << std::endl;
        break;
    }
    case FEE: {
        std::cout << "You owe: $" << userInfo.getFees() << std::endl; // would convert into a decimal rounded to the tenth if this was real.
        userInfo.doWithdraw(userInfo.getFees()); // No need for a seperate payFees() func since no one really gets money, just "withdraw" it.
        std::cout << "Your balance is now: " << userInfo.getBalance() << std::endl;
        break;
    }
    }
    userInfo.save(); // save all the info
}
int main() {
  
    std::cout << bankingLogo << std::endl;

    writeEvent("Welcome to National Banking!");
    writeEvent("What would you like to do today? (Type the number)");

    /* List options, for now we have
    1. Make new account
       - Make user name, password, etc then encrypt and store locally
    2. Login
       - Verify login with decrypted stored login
       - Allow access to sub options like getting and giving money, checking fees, etc.
    */

    writeEvent("1. Make new account.");
    writeEvent("2. Login to existing account.");

    int chosenOption = 0;
    do {
        std::cin >> chosenOption;
    } while (chosenOption <= 0 || chosenOption >= 3);

    if (chosenOption == 1) {
        if (createAccount())
            writeEvent("Account created!");
        else {
            writeEvent("Failed to store or create login!");
            writeEvent("Make sure you have the proper permissions (I.E admin if you have issues)");
        }
    }
    else { // we can use else since we aren't allowing them to this point without using a correct number.
        if (handleLogin()) {
            writeEvent("Login successful!");
            handlePostLogin();
        }
        else {
            writeEvent("Login failed!");
        }
    }
	return 0;
}