/*
 * RuntimeDataManager.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: ylc
 */

#include "RuntimeDataManager.h"

#include "Transfer.h"
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 3)
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#else
#include "llvm/Constants.h"
#endif
#include <iostream>

using namespace std;
using namespace llvm;

namespace klee {

RuntimeDataManager::RuntimeDataManager() :
		currentTrace(NULL) {
	traceList.reserve(20);
	allFormulaNum = 0;
	allGlobal = 0;
	brGlobal = 0;
	solvingTimes = 0;
	satBranch = 0;
	unSatBranch = 0;
	uunSatBranch = 0;
	solvingCost = 0.0;
	runningCost = 0.0;
	satCost = 0.0;
	unSatCost = 0.0;
	runState = 0;

	taintCost = 0;
	PTSCost = 0;
	DTAMCost = 0;
	DTAMSerialCost = 0;
	DTAMParallelCost = 0;
	DTAMhybridCost = 0;

	firstPTSCost = 0;
	firstDTAMCost = 0;
	firstDTAMSerialCost = 0;
	firstDTAMParallelCost = 0;
	firstDTAMhybridCost = 0;

	DTAMSerial = 0;
	DTAMParallel = 0;
	DTAMhybrid = 0;

	taint = 0;
	taintPTS = 0;
	noTaintPTS = 0;
	allTaint = 0;

	firstDTAMSerial = 0;
	firstDTAMParallel = 0;
	firstDTAMhybrid = 0;

	firstTaint = 0;
	firstTaintPTS = 0;
	firstNoTaintPTS = 0;
	firstAllTaint = 0;

	firstDTAMSerialMap = 0;
	firstDTAMParallelMap = 0;
	firstDTAMhybridMap = 0;
	firstTaintMap = 0;
}

RuntimeDataManager::~RuntimeDataManager() {
	for (vector<Trace*>::iterator ti = traceList.begin(), te = traceList.end();
			ti != te; ti++) {
		delete *ti;
	}
	string ErrorInfo;
	raw_fd_ostream out_to_file("./output_info/statics.txt", ErrorInfo, 0x0202);
	stringstream ss;
	ss << "AllFormulaNum:" << allFormulaNum << "\n";
	ss << "SovingTimes:" << solvingTimes << "\n";
	ss << "TotalNewPath:" << testedTraceList.size() << "\n";
	ss << "TotalOldPath:" << traceList.size() - testedTraceList.size() << "\n";
	ss << "TotalPath:" << traceList.size() << "\n";
	if (testedTraceList.size()) {
		ss << "allGlobal:" << allGlobal * 1.0 / testedTraceList.size() << "\n";
		ss << "brGlobal:" << brGlobal * 1.0 / testedTraceList.size() << "\n";
	} else {
		ss << "allGlobal:0" << "\n";
		ss << "brGlobal:0" << "\n";
	}
	if (testedTraceList.size()) {
		ss << "AllBranch:" << ( satBranch + unSatBranch ) * 1.0 / testedTraceList.size() << "\n";
		ss << "satBranch:" << satBranch * 1.0 / testedTraceList.size() << "\n";
	} else {
		ss << "AllBranch:0" << "\n";
		ss << "satBranch:0" << "\n";
	}
	if (satBranch) {
		ss << "satCost:" << satCost / satBranch << "\n";
	} else {
		ss << "satCost:0" << "\n";
	}
	if (testedTraceList.size()) {
		ss << "unSatBranch:" << unSatBranch * 1.0 / testedTraceList.size()  << "\n";
	} else {
		ss << "unSatBranch:0" << "\n";
	}
	if (unSatBranch) {
		ss << "unSatCost:" << unSatCost / unSatBranch << "\n";
	} else {
		ss << "unSatCost:0" << "\n";
	}
	if (testedTraceList.size()) {
		ss << "uunSatBranch:" << uunSatBranch * 1.0 / testedTraceList.size()  << "\n";
	} else {
		ss << "uunSatBranch:0" << "\n";
	}
	ss << "SolvingCost:" << solvingCost << "\n";
	ss << "RunningCost:" << runningCost << "\n";

	ss << "PTSCost:" << PTSCost + taintCost << "\n";
	ss << "DTAMCost:" << DTAMCost + taintCost << "\n";
	ss << "DTAMSerialCost:" << DTAMSerialCost + taintCost << "\n";
	ss << "DTAMParallelCost:" << DTAMParallelCost + taintCost << "\n";
	ss << "DTAMhybridCost:" << DTAMhybridCost + taintCost << "\n";


	ss << "DTAMSerial:" << DTAMSerial << "\n";
	ss << "DTAMParallel:" << DTAMParallel << "\n";
	ss << "DTAMhybrid:" << DTAMhybrid << "\n";

	ss << "taint:" << taint << "\n";
	ss << "taintPTS:" << taintPTS << "\n";
	ss << "noTaintPTS:" << noTaintPTS << "\n";
	ss << "allTaint:" << taint + taintPTS << "\n";

	ss << "firstPTSCost:" << firstPTSCost << "\n";
	ss << "firstDTAMCost:" << firstDTAMCost << "\n";
	ss << "firstDTAMSerialCost:" << firstDTAMSerialCost << "\n";
	ss << "firstDTAMParallelCost:" << firstDTAMParallelCost << "\n";
	ss << "firstDTAMhybridCost:" << firstDTAMhybridCost << "\n";


	ss << "firstDTAMSerial:" << firstDTAMSerial << "\n";
	ss << "firstDTAMParallel:" << firstDTAMParallel << "\n";
	ss << "firstDTAMhybrid:" << firstDTAMhybrid << "\n";

	ss << "firstTaint:" << firstTaint << "\n";
	ss << "firstTaintPTS:" << firstTaintPTS << "\n";
	ss << "firstNoTaintPTS:" << firstNoTaintPTS << "\n";
	ss << "firstAllTaint:" << firstAllTaint << "\n";

	ss << "taintMap:" << taintMap.size() << "\n";
	ss << "DTAMSerialMap:" << DTAMSerialMap.size() << "\n";
	ss << "DTAMParallelMap:" << DTAMParallelMap.size() << "\n";
	ss << "DTAMhybridMap:" << DTAMhybridMap.size() << "\n";

	ss << "firstTaintMap:" << firstTaintMap << "\n";
	ss << "firstDTAMSerialMap:" << firstDTAMSerialMap << "\n";
	ss << "firstDTAMParallelMap:" << firstDTAMParallelMap << "\n";
	ss << "firstDTAMhybridMap:" << firstDTAMhybridMap << "\n";

	out_to_file << ss.str();
	out_to_file.close();
}

Trace* RuntimeDataManager::createNewTrace(unsigned traceId) {
	currentTrace = new Trace();
	currentTrace->Id = traceId;
	traceList.push_back(currentTrace);
	return currentTrace;
}

Trace* RuntimeDataManager::getCurrentTrace() {
	return currentTrace;
}

void RuntimeDataManager::addScheduleSet(Prefix* prefix) {
	scheduleSet.push_back(prefix);
}

void RuntimeDataManager::printCurrentTrace(bool file) {
	currentTrace->print(file);
}

Prefix* RuntimeDataManager::getNextPrefix() {
	//cerr << "prefix num = " << scheduleSet.size() << endl;
	if (scheduleSet.empty()) {
		return NULL;
	} else {
		Prefix* prefix = scheduleSet.front();
		scheduleSet.pop_front();
		return prefix;
	}
}

void RuntimeDataManager::clearAllPrefix() {
	scheduleSet.clear();
}

bool RuntimeDataManager::isCurrentTraceUntested() {
	bool result = true;
	for (set<Trace*>::iterator ti = testedTraceList.begin(), te =
			testedTraceList.end(); ti != te; ti++) {
		if (currentTrace->isEqual(*ti)) {
			result = false;
			break;
		}
	}
	currentTrace->isUntested = result;
	if (result) {
		testedTraceList.insert(currentTrace);
	}
	return result;
}

void RuntimeDataManager::printAllPrefix(ostream &out) {
	out << "num of prefix: " << scheduleSet.size() << endl;
	unsigned num = 1;
	for (list<Prefix*>::iterator pi = scheduleSet.begin(), pe =
			scheduleSet.end(); pi != pe; pi++) {
		out << "Prefix " << num << endl;
		(*pi)->print(out);
		num++;
	}
}

void RuntimeDataManager::printAllTrace(ostream &out) {
	out << "\nTrace Info:\n";
	out << "num of trace: " << traceList.size() << endl << endl;
	unsigned num = 1;
	for (vector<Trace*>::iterator ti = traceList.begin(), te = traceList.end();
			ti != te; ti++) {
		Trace* trace = *ti;
		if (trace->isUntested) {
			out << "Trace " << num << endl;
			if (trace->abstract.empty()) {
				trace->createAbstract();
			}
			for (vector<string>::iterator ai = trace->abstract.begin(), ae =
					trace->abstract.end(); ai != ae; ai++) {
				out << *ai << endl;
			}
			out << endl;
			num++;
		}
	}
}

}
