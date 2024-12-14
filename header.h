#include<iostream>
#include<string>
#include<fstream>

using namespace std;

struct Packet {
    string src;
    string dst;
    string protocol;
    string data;
    int no_of_packets;
};

struct Rules {
    string src;
    string dst;
    string pro;
    string action;
    int rule_no;
};

Packet noofpack;
Rules applyrules[100];
Packet pack[100];
int k = 0;
int l = 0;

void storerules(string& rulestext, int i) {
    int posrule_no = rulestext.find(" ");
    string sub_rule_no = rulestext.substr(0, posrule_no);
    applyrules[i].rule_no = stoi(sub_rule_no);

    int pos_src = rulestext.find("SRC ") + 4;
    string sub_src = rulestext.substr(pos_src, rulestext.find(" ", pos_src) - pos_src);
    applyrules[i].src = sub_src;

    int pos_dst = rulestext.find(" DST ") + 5;
    string sub_dst = rulestext.substr(pos_dst, rulestext.find(" ", pos_dst) - pos_dst);
    applyrules[i].dst = sub_dst;

    int pos_pro = rulestext.find(" PRO ") + 5;
    string sub_pro = rulestext.substr(pos_pro, rulestext.find(" ", pos_pro) - pos_pro);
    applyrules[i].pro = sub_pro;
}

void rulefile() {
    string rulestext;
    ifstream rules_file("rule.txt");

    while (getline(rules_file, rulestext)) {
        storerules(rulestext, l);
        l++;
    }
}


// ... (Previous code remains unchanged)

void packets() {
    string text, line;
    ifstream input("sample.txt");
    while (getline(input, line)) {
        text += line;
    }

    int start = 0;
    int i = 0;

    while (true) {
        start = text.find('[', start);
        if (start == -1) {  // Change string::npos to -1
            break;
        }
        int end = text.find(']', start);
        if (end == -1) {  // Change string::npos to -1
            break;
        }
        string pk = text.substr(start, end - start + 1);
        noofpack.no_of_packets++;

        int pos = pk.find("SRC:") + 4;
        pack[k].src = pk.substr(pos, pk.find(";", pos) - pos);

        pos = pk.find("D") + 4;
        pack[k].dst = pk.substr(pos, pk.find(';', pos) - pos);

        pos = pk.find("O:") + 2;
        pack[k].protocol = pk.substr(pos, pk.find(';', pos) - pos);

        pos = pk.find_last_of(';') + 1;
        pack[k].data = pk.substr(pos, pk.find(']', pos) - pos);

        k++;
        start = end + 1;
    }
    input.close();
}

// void compare() {
//     ofstream outputfile("result.dat");
//     if (!outputfile.is_open()) {
//         cout << "error while opening the file!\n";
//     }

//     for (int i = 0; i < k; i++) {
//         bool packetblocked = true;
//         for (int j = 0; j < l; j++) {
//             int o = applyrules[j].dst.find("-");
//             if (o != -1 && pack[i].src == applyrules[j].src && pack[i].protocol == applyrules[j].pro) {  // Change string::npos to -1
//                 packetblocked = false;

//                 int u = applyrules[j].dst.find_last_of(".") + 1;
//                 int brange = stoi(applyrules[j].dst.substr(u, applyrules[j].dst.find("-", u) - u));
//                 int arange = stoi(applyrules[j].dst.substr(applyrules[j].dst.find("-") + 1));

//                 int p = pack[i].dst.find_last_of(".") + 1;
//                 int rp = stoi(pack[i].dst.substr(p));

//                 if (brange <= rp && arange >= rp) {
//                     if (applyrules[j].action == "ALLOW") {
//                         outputfile << "SRC " << pack[i].src << " DST " << pack[i].dst << " PRO " << pack[i].protocol << " ALLOW " << j + 1 << endl;
//                     } else if (applyrules[j].action == "DENY") {
//                         outputfile << "SRC " << pack[i].src << " DST " << pack[i].dst << " PRO " << pack[i].protocol << " DENY " << j + 1 << endl;
//                     }
//                 } else {
//                     outputfile << "SRC " << pack[i].src << " DST " << pack[i].dst << " PRO " << pack[i].protocol << " DENY " << j + 1 << endl;
//                 }
//             }

//             if (pack[i].src == applyrules[j].src && pack[i].dst == applyrules[j].dst && pack[i].protocol == applyrules[j].pro) {
//                 packetblocked = false;
//                 if (applyrules[j].action == "ALLOW") {
//                     outputfile << "SRC " << pack[i].src << " DST " << pack[i].dst << " PRO " << pack[i].protocol << " ALLOW " << j + 1 << endl;
//                 } else if (applyrules[j].action == "DENY") {
//                     outputfile << "SRC " << pack[i].src << " DST " << pack[i].dst << " PRO " << pack[i].protocol << " DENY " << j + 1 << endl;
//                 }
//             }
//         }
//         if (packetblocked == true) {
//             outputfile << "SRC " << pack[i].src << " DST " << pack[i].dst << " PRO " << pack[i].protocol << " DENY "  << endl;
//         }
//     }

//     outputfile << "\n\n\n\n\n\n\n_______________________________________________________________________________________________________\n";
//     outputfile << "\n";
// }





void compare() {
    ofstream outputfile("result.dat");
    if (!outputfile.is_open()) {
        cout << "Error while opening the file!\n";
        return;
    }

    for (int i = 0; i < k; i++) {
        bool packetAllowed = false;
        for (int j = 0; j < l; j++) {
            if (applyrules[j].src == pack[i].src && applyrules[j].action == "ALLOW") {
                packetAllowed = true;
                outputfile << "SRC " << pack[i].src << " DST " << pack[i].dst << " PRO " << pack[i].protocol << " ALLOW " << applyrules[j].rule_no << endl;
                break;
            } else if (applyrules[j].dst.find("-") != string::npos) {
                string dst_rule = applyrules[j].dst.substr(4); // Removes "DST " from the rule
                string dst_ip = pack[i].dst.substr(0, dst_rule.find(" "));
                if (dst_ip == dst_rule && applyrules[j].action == "DENY") {
                    packetAllowed = false;
                    break;
                }
            } else if (applyrules[j].pro == pack[i].protocol && applyrules[j].action == "DENY") {
                packetAllowed = false;
                break;
            }
        }

        if (!packetAllowed) {
            outputfile << "SRC " << pack[i].src << " DST " << pack[i].dst << " PRO " << pack[i].protocol << " DENY unspecified\n";
        }
    }

    outputfile << "\n\n\n\n\n\n\n_______________________________________________________________________________________________________\n";
    outputfile << "\n";
}
