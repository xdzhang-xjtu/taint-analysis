/*
 * RuntimeDataManager.h
 *
 *  Created on: Jun 10, 2014
 *      Author: ylc
 */

#ifndef RUNTIMEDATAMANAGER_H_
#define RUNTIMEDATAMANAGER_H_

#include "Trace.h"
#include "Prefix.h"
#include <set>
namespace klee {

class RuntimeDataManager {

private:
	std::vector<Trace*> traceList; // store all traces;
	Trace* currentTrace; // trace associated with current execution
	std::set<Trace*> testedTraceList; // traces which have been examined
	std::list<Prefix*> scheduleSet; // prefixes which have not been examined

public:
	//newly added stastic info
	unsigned allFormulaNum;
	unsigned allGlobal;
	unsigned brGlobal;
	unsigned solvingTimes;
	unsigned satBranch;
	unsigned unSatBranch;
	unsigned uunSatBranch;
	double runningCost;
	double solvingCost;
	double satCost;
	double unSatCost;

	unsigned runState;

	double taintCost;
	double PTSCost;
	double DTAMCost;
	double DTAMSerialCost;
	double DTAMParallelCost;
	double DTAMhybridCost;

	unsigned DTAMSerial;
	unsigned DTAMParallel;
	unsigned DTAMhybrid;

	unsigned taint;
	unsigned taintPTS;
	unsigned noTaintPTS;
	unsigned allTaint;

	double firstPTSCost;
	double firstDTAMCost;
	double firstDTAMSerialCost;
	double firstDTAMParallelCost;
	double firstDTAMhybridCost;

	unsigned firstDTAMSerial;
	unsigned firstDTAMParallel;
	unsigned firstDTAMhybrid;

	unsigned firstTaint;
	unsigned firstTaintPTS;
	unsigned firstNoTaintPTS;
	unsigned firstAllTaint;

	unsigned firstDTAMSerialMap;
	unsigned firstDTAMParallelMap;
	unsigned firstDTAMhybridMap;
	unsigned firstTaintMap;

	RuntimeDataManager();
	virtual ~RuntimeDataManager();

	Trace* createNewTrace(unsigned traceId);
	Trace* getCurrentTrace();
	void addScheduleSet(Prefix* prefix);
	void printCurrentTrace(bool file);
	Prefix* getNextPrefix();
	void clearAllPrefix();
	bool isCurrentTraceUntested();
	void printAllPrefix(std::ostream &out);
	void printAllTrace(std::ostream &out);

	std::set<unsigned> taintMap;
	std::set<unsigned> DTAMSerialMap;
	std::set<unsigned> DTAMParallelMap;
	std::set<unsigned> DTAMhybridMap;
};

}
#endif /* RUNTIMEDATAMANAGER_H_ */
