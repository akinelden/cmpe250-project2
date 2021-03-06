#include "Simulation1.h"
using namespace std;

SIMULATION1::SIMULATION1()
{
	CLOCK = luggage_number = security_number = free_luggage = free_security = 0;
	Online = VIP = 0;
	records.missed_flight = 0;
	records.waiting_time = 0;
}

SIMULATION1::SIMULATION1(bool _Online, bool _VIP, int _l, int _s, vector<CUSTOMER*>* arrival) : Online(_Online), VIP(_VIP), luggage_number(_l), security_number(_s), arrivals(arrival), free_luggage(_l), free_security(_s), CLOCK(0)
{
	it = arrival->begin();
	records.missed_flight = 0;
	records.waiting_time = 0;
}

SIMULATION1::~SIMULATION1()
{
	while (!myevents.empty()) {
		delete myevents.top();
		myevents.pop();
	}
}

SIMULATION1::SIMULATION1(const SIMULATION1 & sim)
{
	CLOCK = sim.CLOCK;
	luggage_number = sim.luggage_number;
	security_number = sim.security_number;
	free_luggage = sim.free_luggage;
	free_security = sim.free_security;
	Online = sim.Online;
	VIP = sim.VIP;
	while (!myevents.empty()) {
		delete myevents.top();
		myevents.pop();
	}
	myevents = sim.myevents;
	while (!luggage_queue.empty()) {
		luggage_queue.pop();
	}
	while (!security_queue.empty()) {
		security_queue.pop();
	}
	luggage_queue = sim.luggage_queue;
	security_queue = sim.security_queue;
	records = sim.records;
	it = sim.it;
	arrivals = sim.arrivals;

}

SIMULATION1 & SIMULATION1::operator=(const SIMULATION1 & sim)
{
	CLOCK = sim.CLOCK;
	luggage_number = sim.luggage_number;
	security_number = sim.security_number;
	free_luggage = sim.free_luggage;
	free_security = sim.free_security;
	Online = sim.Online;
	VIP = sim.VIP;
	while (!myevents.empty()) {
		delete myevents.top();
		myevents.pop();
	}
	myevents = sim.myevents;
	while (!luggage_queue.empty()) {
		luggage_queue.pop();
	}
	while (!security_queue.empty()) {
		security_queue.pop();
	}
	luggage_queue = sim.luggage_queue;
	security_queue = sim.security_queue;
	records = sim.records;
	it = sim.it;
	arrivals = sim.arrivals;

	return *this;
}

SIMULATION1::SIMULATION1(SIMULATION1 && sim)
{
	CLOCK = move(sim.CLOCK);
	luggage_number = move(sim.luggage_number);
	security_number = move(sim.security_number);
	free_luggage = move(sim.free_luggage);
	free_security = move(sim.free_security);
	Online = move(sim.Online);
	VIP = move(sim.VIP);
	while (!myevents.empty()) {
		delete myevents.top();
		myevents.pop();
	}
	myevents = move(sim.myevents);
	while (!luggage_queue.empty()) {
		luggage_queue.pop();
	}
	while (!security_queue.empty()) {
		security_queue.pop();
	}
	luggage_queue = move(sim.luggage_queue);
	security_queue = move(sim.security_queue);
	records = move(sim.records);
	it = move(sim.it);
	arrivals = move(sim.arrivals);
	while (!sim.myevents.empty()) {
		sim.myevents.pop();
	}
	while (!sim.luggage_queue.empty()) {
		sim.luggage_queue.pop();
	}
	while (!sim.security_queue.empty()) {
		sim.security_queue.pop();
	}
	sim.arrivals = nullptr;
}

SIMULATION1 & SIMULATION1::operator=(SIMULATION1 && sim)
{
	CLOCK = move(sim.CLOCK);
	luggage_number = move(sim.luggage_number);
	security_number = move(sim.security_number);
	free_luggage = move(sim.free_luggage);
	free_security = move(sim.free_security);
	Online = move(sim.Online);
	VIP = move(sim.VIP);
	while (!myevents.empty()) {
		delete myevents.top();
		myevents.pop();
	}
	myevents = move(sim.myevents);
	while (!luggage_queue.empty()) {
		luggage_queue.pop();
	}
	while (!security_queue.empty()) {
		security_queue.pop();
	}
	luggage_queue = move(sim.luggage_queue);
	security_queue = move(sim.security_queue);
	records = move(sim.records);
	it = move(sim.it);
	arrivals = move(sim.arrivals);
	while (!sim.myevents.empty()) {
		sim.myevents.pop();
	}
	while (!sim.luggage_queue.empty()) {
		sim.luggage_queue.pop();
	}
	while (!sim.security_queue.empty()) {
		sim.security_queue.pop();
	}
	sim.arrivals = nullptr;

	return *this;
}

void SIMULATION1::Start()
{
	CLOCK = 0;
	ScheduleNextArrival();
	while (!myevents.empty()) {
		NextEvent();
	}
}

Record SIMULATION1::Results()
{
	Record res;
	res.missed_flight = records.missed_flight;
	res.waiting_time = records.waiting_time / double(arrivals->size());
	return res;
}

void SIMULATION1::ScheduleNextArrival()
{
	if (it == (*arrivals).end())
		return;
	CUSTOMER* temp = *it;
	it++;
	int x = temp->GetArrivalTime();
	EVENT* e = new EVENT(x, 0, temp);
	myevents.push(e);
}

void SIMULATION1::NextEvent()
{
	EVENT* temp = move(myevents.top());
	myevents.pop();
	temp->Happen(this);
	delete temp;
}

void SIMULATION1::MoveToLuggageQueue(CUSTOMER *& c, const int & time)
{
	CLOCK = time;
	if (Online && c->IsOnline()) {
		MoveToSecurityQueue(c, time);
		return;
	}
	else if (luggage_queue.empty() && free_luggage) {
		int x = CLOCK + c->GetLuggageTime();
		EVENT* e = new EVENT(x, 1, c);
		myevents.push(e);
		free_luggage--;
		return;
	}
	else {
		luggage_queue.push(c);
	}
}

void SIMULATION1::MoveToSecurityQueue(CUSTOMER *& c, const int & time)
{
	CLOCK = time;
	if (VIP && c->IsVIP()) {
		EnterRecords(c, time);
		return;
	}
	else if (security_queue.empty() && free_security) {
		int x = CLOCK + c->GetSecurityTime();
		EVENT* e = new EVENT(x, 2, c);
		myevents.push(e);
		free_security--;
		return;
	}
	else {
		security_queue.push(c);
	}
}

void SIMULATION1::EnterRecords(CUSTOMER *& c, const int & time)
{
	CLOCK = time;
	Record rec;
	int w = CLOCK - c->GetArrivalTime();
	int m;
	if (CLOCK > c->GetFlightTime())
		m = 1;
	else m = 0;
	records.missed_flight += m;
	records.waiting_time += w;
	int t = 0;
}

void SIMULATION1::CallLuggageCustomer()
{
	free_luggage++;
	if (luggage_queue.empty())
		return;
	else {
		CUSTOMER* c = luggage_queue.top();
		luggage_queue.pop();
		int x = CLOCK + c->GetLuggageTime();
		EVENT* e = new EVENT(x, 1, c);
		myevents.push(e);
		free_luggage--;
	}
}

void SIMULATION1::CallSecurityCustomer()
{
	free_security++;
	if (security_queue.empty())
		return;
	else {
		CUSTOMER* c = security_queue.top();
		security_queue.pop();
		int x = CLOCK + c->GetSecurityTime();
		EVENT* e = new EVENT(x, 2, c);
		myevents.push(e);
		free_security--;
	}
}

