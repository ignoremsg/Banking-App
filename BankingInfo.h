#pragma once
#include <string>
#include <fstream>
#include <vector>
class BankingInfo {
public:
	BankingInfo() {
		_money = 0.0f;
		_fees = 0.0f;
	}
	BankingInfo(float money, float fees, std::string user, std::string pass) {
		_money = money;
		_fees = fees;
	}
	float getBalance() {
		return _money;
	}
	void setBalance(float balance) {
		_money = balance;
	}
	void doWithdraw(float toTake) {
		_money -= toTake;
		// could add some form of balance validation to make sure they have that amount.
	}
	void doDeposit(float toAdd) {
		_money += toAdd;
	}
	float getFees() {
		return _fees;
	}
	float getBalanceWithFees() {
		return _money - _fees;
	}
	void resetData() {
		_money = 0.0f;
		_fees = 0.0f;
	}
	bool save() {
		std::fstream save{ "bankInfo.txt", std::fstream::trunc | std::fstream::out };
		if (!save.is_open() || save.fail())
			return false;
		save << _money << "\n";
		save << _fees << "\n";
		save.close();
		return true;
	}
	BankingInfo load() {
		std::ifstream load{ "bankInfo.txt"};
		BankingInfo info;
		if (!load.is_open() || load.fail())
			return info; // empty info
		
		float moneyz[2] = { 0.0f };
		int lines = 0;
		std::string line;
		while (std::getline(load, line)) {
			moneyz[lines] = std::stof(line);
			lines++;
		}
		info._money = moneyz[0];
		info._fees = moneyz[1];
		load.close();
		return info;
	}
	bool didLoad = false;
private:
	float _money, _fees;
};