#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


using namespace std;


class coordinate_pair {
public:
	int x_coordinate;
	int y_coordinate;
	coordinate_pair() :x_coordinate(0), y_coordinate(0) {}
	coordinate_pair(int x, int y) :x_coordinate(x), y_coordinate(y) {}
};

class Entity {
private:
	void ability_checks() {   //Bütün özellikleri kontrol edip işaretler
		int counter = 0;

		for (int i = 0; i < gates.size(); i++) {
			if (gates[i] == 'A' || gates[i] == 'B') {
				can_be_infected = true;
				break;
			}
		}

		for (int i = 0; i < gates.size(); i++) {
			if (gates[i] == 'A' || gates[i] == 'B' || gates[i] == 'C' || gates[i] == 'D') {
				counter += 1;
			}
		}

		if (counter >= 3) {
			can_die = true;
		}

		if (can_die == false) {
			can_recover = true;
		}

	}
public:
	bool can_die;
	bool dead;
	bool can_recover;
	bool recovered;
	bool can_be_infected;
	bool infected;
	bool erased_from_habitat;
	int dead_x_coor;
	int dead_y_coor;
	int V_spreading_rate; //bu değişken kaç tur boyunca virüslü kaldığını tutar
	int x_coor;
	int y_coor;
	string gates;
	
	Entity(int x, int y, string ID, bool infected) :x_coor(x), y_coor(y), gates(ID), infected(infected),
		can_be_infected(false), can_die(false), recovered(false), can_recover(false), erased_from_habitat(false),
		V_spreading_rate(0), dead(false) {
		ability_checks();
	}

	Entity() :x_coor(0), y_coor(0), gates(""), infected(false), can_be_infected(false), can_die(false), recovered(false)
		, can_recover(false), V_spreading_rate(0), dead(false), erased_from_habitat(false) {
		ability_checks();
	}
};

class class_Habitat {
public:
	vector<vector<char>> arr;
	vector<vector<Entity*>> ptr_arr;
	vector<Entity*> all_entity;
	int size;


	void reset_ptr_arr() {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				ptr_arr[i][j] = nullptr;
			}
		}
	}

	void reset_arr() {
		for (int i = 0; i < size; i++) {
			arr[0][i] = '-';
			arr[size - 1][i] = '-';
			arr[i][0] = '-';
			arr[i][size - 1] = '-';
		}

		for (int i = 1; i < size - 1; i++) {
			for (int j = 1; j < size - 1; j++) {
				arr[i][j] = ' ';
			}
		}
	}
	class_Habitat(int siz) {
		siz += 2;
		size = siz;
		
		for (int i = 0; i < siz; i++) {
			arr.push_back(vector<char>(siz));
		}
		
		for (int i = 0; i < siz; i++) {
			ptr_arr.push_back(vector<Entity*>(siz));
		}
		
		reset_arr();
		reset_ptr_arr();

	}
};

void read_habitat_constants(int&, int&, ifstream&);
void read_entities(class_Habitat&, ifstream&, string&);
void turns(ifstream&, ofstream&, int, class_Habitat&, string&);
void seperate_definition_coordinate(string&, int&, int&);
void rezul(vector<string>&, string&);
void pin_square(Entity*, int&, int&, int&, int&, int);
void write_output(ofstream&, class_Habitat&);

void write_output(ofstream& output, class_Habitat& habitat) {
	int normal_count = 0;
	int recovered_count = 0;
	int infected_count = 0;
	int dead_count = 0;

	for (int i = 0; i < habitat.all_entity.size(); i++) {
		if (habitat.all_entity[i]->dead == true || habitat.all_entity[i]->erased_from_habitat == true) {
			dead_count += 1;
		}

		else if (habitat.all_entity[i]->infected == true) {
			infected_count += 1;
		}

		else if (habitat.all_entity[i]->recovered == true) {
			recovered_count += 1;
		}

		else if (habitat.all_entity[i]->infected == false || habitat.all_entity[i]->dead == false) {
			normal_count += 1;
		}

	}

	output << "Normal     : " << normal_count << "\n"
		<< "Infected   : " << infected_count << "\n"
		<< "Dead       : " << dead_count << "\n"
		<< "Recovered  : " << recovered_count << "\n";



	for (int i = 0; i < habitat.all_entity.size(); i++) {

		if (habitat.all_entity[i]->dead == true || habitat.all_entity[i]->erased_from_habitat == true) { //ölü
			output << "entity " << i + 1 << " " << habitat.all_entity[i]->dead_x_coor << "x" 
				<< habitat.all_entity[i]->dead_y_coor
				<< " " << "dead\n";
		}

		else if (habitat.all_entity[i]->infected == true) {  // hastalık kapmış
			output << "entity " << i + 1 << " " << habitat.all_entity[i]->x_coor << "x" << habitat.all_entity[i]->y_coor
				<< " " << "infected\n";
		}

		else if (habitat.all_entity[i]->recovered == true) {         //iyileşmiş
			output << "entity " << i + 1 << " " << habitat.all_entity[i]->x_coor << "x" << habitat.all_entity[i]->y_coor
				<< " " << "immune\n";
		}


		else if (habitat.all_entity[i]->infected == false && habitat.all_entity[i]->dead == false) {      //normal
			output << "entity " << i + 1 << " " << habitat.all_entity[i]->x_coor << "x" << habitat.all_entity[i]->y_coor
				<< " " << "normal\n";
		}
	}

}


void pin_square(Entity* entity, int& A_edge, int& B_edge, int& C_edge, int& D_edge, int size) {
	if (entity->x_coor - 3 < 1) {
		A_edge = 1;
	}
	else {
		A_edge = entity->x_coor - 3;
	}

	if (entity->x_coor + 3 > size - 2) {
		B_edge = size - 2;
	}
	else {
		B_edge = entity->x_coor + 3;
	}

	if (entity->y_coor - 3 < 1) {
		C_edge = 1;
	}
	else {
		C_edge = entity->y_coor - 3;
	}

	if (entity->y_coor + 3 > size - 2) {
		D_edge = size - 2;
	}
	else {
		D_edge = entity->y_coor + 3;
	}
}


void turns(ifstream& input_read, ofstream& turns_write, int turn_count, class_Habitat& habitat, string& crta) {
	vector<string> rijeci;
	vector<coordinate_pair> container;
	int x_coordinate = 0;
	int y_coordinate = 0;
	int A_corner = 0, B_corner = 0, C_corner = 0, D_corner = 0;
	int counter = 0;
	int habitat_all_entity_size = habitat.all_entity.size();
	int flag = 0;

	for (int l = 1; l <= turn_count; l++) {



		/*______________________Increment_________________*/

		for (int i = 0; i < habitat_all_entity_size; i++) {
			if (habitat.all_entity[i]->infected == true || habitat.all_entity[i]->dead == true) {
				habitat.all_entity[i]->V_spreading_rate += 1;
			}
		}

		/*___________________Control______________________*/


		for (int i = 0; i < habitat_all_entity_size; i++) {
			if (habitat.all_entity[i]->can_die == true && habitat.all_entity[i]->V_spreading_rate >= 15 && //ölüm
				habitat.all_entity[i]->infected == true) {
				habitat.all_entity[i]->dead = true;
				habitat.all_entity[i]->infected = false;
				habitat.all_entity[i]->erased_from_habitat = false;
				habitat.all_entity[i]->recovered = false;
				habitat.all_entity[i]->dead_x_coor = habitat.all_entity[i]->x_coor;
				habitat.all_entity[i]->dead_y_coor = habitat.all_entity[i]->y_coor;
				continue;
			}

			if (habitat.all_entity[i]->can_recover == true && habitat.all_entity[i]->V_spreading_rate >= 31 && //kurtulma
				habitat.all_entity[i]->infected == true) {
				habitat.all_entity[i]->recovered = true;
				habitat.all_entity[i]->infected = false;
				habitat.all_entity[i]->dead = false;
				habitat.all_entity[i]->erased_from_habitat = false;
				habitat.all_entity[i]->V_spreading_rate = 0;
				continue;
			}

			if (habitat.all_entity[i]->dead == true && habitat.all_entity[i]->V_spreading_rate >= 20) { //ölüneni silme
				habitat.all_entity[i]->erased_from_habitat = true;
				habitat.all_entity[i]->dead = false;
				habitat.all_entity[i]->infected = false;
				habitat.all_entity[i]->recovered = false;
				continue;
			}
		}

		for (int i = 0; i < habitat_all_entity_size; i++) {   //enfekte etme

			if (habitat.all_entity[i]->infected == true && habitat.all_entity[i]->V_spreading_rate <= 3) {
				continue;
			}

			else if (habitat.all_entity[i]->infected == false &&
				habitat.all_entity[i]->dead == false &&
				habitat.all_entity[i]->erased_from_habitat == false &&
				habitat.all_entity[i]->recovered == false) {
				continue;
			}

			else if (habitat.all_entity[i]->infected == false &&
				habitat.all_entity[i]->dead == false &&
				habitat.all_entity[i]->erased_from_habitat == false &&
				habitat.all_entity[i]->recovered == true) {
				continue;
			}

			else if (habitat.all_entity[i]->erased_from_habitat == true) {
				continue;
			}
			pin_square(habitat.all_entity[i], A_corner, B_corner, C_corner, D_corner, habitat.size);

			for (int j = A_corner; j <= B_corner; j++) {

				for (int k = C_corner; k <= D_corner; k++) {
					if (habitat.ptr_arr[j][k] != nullptr) {

						if (habitat.ptr_arr[j][k]->can_be_infected == true &&
							habitat.ptr_arr[j][k]->recovered == false &&
							habitat.ptr_arr[j][k]->dead == false &&
							habitat.ptr_arr[j][k]->erased_from_habitat == false &&
							habitat.ptr_arr[j][k]->infected == false) {
							habitat.ptr_arr[j][k]->infected = true;

						}
					}
				}
			}
		}

		/*_____________________Apply_______________*/

		container.clear();


		rezul(rijeci, crta);

		for (int j = 2; j < rijeci.size(); j++) {
			seperate_definition_coordinate(rijeci[j], x_coordinate, y_coordinate);
			container.push_back(coordinate_pair(x_coordinate, y_coordinate));
		}

		for (int i = 0; i < habitat_all_entity_size; i++) {
			if (habitat.all_entity[i]->dead == false && habitat.all_entity[i]->erased_from_habitat == false) {
				habitat.all_entity[i]->x_coor = container[i].x_coordinate;
				habitat.all_entity[i]->y_coor = container[i].y_coordinate;
			}
		}
		if (input_read.eof() == false) {
			getline(input_read, crta);
		}
		/*____________________Write_Turn_____________________________*/

		habitat.reset_ptr_arr();   //üstten geldiği için burada

		for (int i = 0; i < habitat_all_entity_size; i++) {
			flag = 0;

			if (habitat.all_entity[i]->infected == true) {
				habitat.arr[habitat.all_entity[i]->x_coor][habitat.all_entity[i]->y_coor] = 'X';
				habitat.ptr_arr[habitat.all_entity[i]->x_coor][habitat.all_entity[i]->y_coor] = habitat.all_entity[i];
				flag = 1;
			}

			if (habitat.all_entity[i]->dead == true) {
				habitat.arr[habitat.all_entity[i]->x_coor][habitat.all_entity[i]->y_coor] = 'D';
				habitat.ptr_arr[habitat.all_entity[i]->x_coor][habitat.all_entity[i]->y_coor] = habitat.all_entity[i];
				flag = 1;
			}

			if (habitat.all_entity[i]->recovered == true) {
				habitat.arr[habitat.all_entity[i]->x_coor][habitat.all_entity[i]->y_coor] = '0';
				habitat.ptr_arr[habitat.all_entity[i]->x_coor][habitat.all_entity[i]->y_coor] = habitat.all_entity[i];
				flag = 1;
			}


			if (habitat.all_entity[i]->erased_from_habitat == true) {
				continue;
			}

			if (flag == 0) {
				habitat.arr[habitat.all_entity[i]->x_coor][habitat.all_entity[i]->y_coor] = 'O';
				habitat.ptr_arr[habitat.all_entity[i]->x_coor][habitat.all_entity[i]->y_coor] = habitat.all_entity[i];
			}

		}

		counter++;

		turns_write << "Turn " << counter << ":\n";
		for (int i = 0; i < habitat.size; i++) {
			for (int j = 0; j < habitat.size; j++) {
				turns_write << habitat.arr[i][j];
			}
			turns_write << '\n';
		}

		habitat.reset_arr();
	}
}



void seperate_definition_coordinate(string& word, int& x_coordinate, int& y_coordinate) {
	int flag = 0;
	string temp = "";

	for (int i = 0; i < word.size(); i++) {
		if (word[i] == 'x') {
			flag = 1;
			continue;
		}
		if (flag == 1) {
			temp.push_back(word[i]);
		}
	}

	x_coordinate = stoi(word);
	y_coordinate = stoi(temp);
}

void rezul(vector<string>& rijeci,string& crta) 
{
	rijeci.clear();
	istringstream iss(crta);
	string word;
	while (iss >> word) {
		rijeci.push_back(word);
	}
}




void read_habitat_constants(int& size, int& turn_count, ifstream& input_read) {
	string crta = "";
	int read_number = 0;
	int counter = 0;
	int cursor = 0;
	vector<string> rijeci;

	getline(input_read, crta);										
	rezul(rijeci, crta);								
	size = stoi(rijeci[1]);

	rijeci.clear();
	getline(input_read, crta);
	rezul(rijeci, crta);
	turn_count = stoi(rijeci[1]);
}



void read_entities(class_Habitat& habitat, ifstream& input_read, string& crta) {

	int x_coordinate = 0;
	int y_coordinate = 0;
	int counter = 0;

	string gates;
	vector<string> rijeci;

	do {
		getline(input_read, crta);
		rezul(rijeci, crta);
		if (rijeci[0] == "turn") {
			break;
		}
		gates = rijeci[2];
		if (rijeci.size() == 5) {
			seperate_definition_coordinate(rijeci[3], x_coordinate, y_coordinate);
		}
		else {
			seperate_definition_coordinate(rijeci[3], x_coordinate, y_coordinate);
		}
		if (rijeci.size() == 5) {
			habitat.all_entity.push_back(new Entity(x_coordinate, y_coordinate, gates, true));
		}
		else {

			habitat.all_entity.push_back(new Entity(x_coordinate, y_coordinate, gates, false));
		}
		counter += 1;
	} while (rijeci[0] != "turn");

}


int main() {
	ifstream input_read("INPUT.txt", ios::in);
	ofstream turn_write("TURNS.txt", ios::out);
	int size = 0, turn_count = 0, entity_count = 0;
	string crta = "";

	read_habitat_constants(size, turn_count, input_read);

	class_Habitat habitat(size);

	read_entities(habitat, input_read, crta);

	for (int i = 0; i < habitat.all_entity.size(); i++) {
		habitat.ptr_arr[habitat.all_entity[i]->x_coor][habitat.all_entity[i]->y_coor] = habitat.all_entity[i];
	}

	turns(input_read, turn_write, turn_count, habitat, crta);

	input_read.close();
	turn_write.close();
	turn_write.open("OUTPUT.txt", ios::out);

	write_output(turn_write, habitat);

	return 0;
}