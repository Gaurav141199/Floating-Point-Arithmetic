#include <iostream> 
#include <fstream>
#include <cmath>
#define deb(x) cout << #x << " = " << x << endl
#define deb2(x, y) cout << #x << " = " << x << "," << #y << " = " << y << endl

using namespace std;

// challenges involved->
// 1) understand representation of floating numbers in double precision 
// 2) understand the algorithm 
// 3) understand how to solve the overflow and underflow conditons

long long int binary_to_int(string num){
    int val = num.size();
    long long int res = 0;

    for(int i = val - 1; i >= 0; i--)
        if(num[i] == '1') res += pow(2, val - 1 - i);

    return res;
}

int exponent_diff(string exp1, string exp2){
    return binary_to_int(exp1) - binary_to_int(exp2);
}

string int_to_binary25(long long int num){
    int binaryNum[25]; 

    long long int n = num;

    int i = 0; 
    while (n > 0) {  
        binaryNum[i] = n % 2; 
        n = n / 2; 
        i++; 
    } 

    string res = "";
  
    for (int j = i - 1; j >= 0; j--) 
        res = res + to_string(binaryNum[j]); 

    int size = res.size();

    for(int k = 0; k < 25 - size; ++k)
    	res = "0" + res;

    return res;
}

void shift_fraction(string &fraction, int val){
    string res = "";
    for (int i = 0; i < val; i++){
        if(i == val - 1) res = res + '1';
        else res = res + '0';
    }        
    
    res = res + fraction;

    string ret = "";

    for(int i = 0; i < 23; ++i)
        ret = ret + res[i];

    if(fraction[fraction.size() - val] == '1'){
        int val = binary_to_int(ret) + 1;
        string result = int_to_binary25(val);
        fraction = result;
        return;
    }

    fraction = ret;
}

string int_to_binary8(long long int num){
    int binaryNum[8]; 

    long long int n = num;

    int i = 0; 
    while (n > 0) {  
        binaryNum[i] = n % 2; 
        n = n / 2; 
        i++; 
    } 

    string res = "";
  
    for (int j = i - 1; j >= 0; j--) 
        res = res + to_string(binaryNum[j]); 

    int size = res.size();

    for(int k = 0; k < 8 - size; ++k)
    	res = "0" + res;

    return res;
}

string significand_sum(string f1, char sign1, string f2, char sign2, int shift_value){

    string fraction1 = f1;
    string fraction2 = f2;

    if(shift_value < 0) {
        fraction2 = '1' + fraction2;
        fraction1 = '0' + fraction1;
    }
    else if(shift_value > 0){
        fraction2 = '0' + fraction2;
        fraction1 = '1' + fraction1;  
    } 
    else {
        fraction1 = "1" + fraction1;
        fraction2 = "1" + fraction2;
    }  


    long long int frac1 = binary_to_int(fraction1);
    if(sign1 == '1') frac1 = frac1*(-1);

    long long int frac2 = binary_to_int(fraction2);
    if(sign2 == '1') frac2 = frac2*(-1);

    long long sum = frac1 + frac2;

    string res = "";

    if(sum < 0) res = int_to_binary25(sum*-1);
    else res = int_to_binary25(sum);

    char sign = '0';
    if(sum < 0) sign = '1';

    res = sign + res; 

    return res;
}


int normalise(string &fraction, int &exp, int &cycles){
    int pos;
    for(int i = 0; i < fraction.size(); ++i){
        if(fraction[i] == '1') {
            pos = i;
            break;
        }
    }    

    int diff = pos - 1;

    if(diff < 0){
        cycles++;
        if(fraction[fraction.size() - 1] == '0'){
            fraction = fraction.substr(0, 24);
            fraction = '0' + fraction;
            exp++;
            if(exp > 254){
                cout<<"OVERFLOW"<<endl;
                cout<<"number of cycles: "<<cycles<<endl;
                return -1;
            }
        }else{
            //rounding off 
            fraction = fraction.substr(0, 24);
            fraction = '0' + fraction;
            int val = binary_to_int(fraction);
            val = val + 1;
            fraction = int_to_binary25(val);
            exp++;
            cycles++;
        }             
    }
    else if(diff > 0){
        cycles++;
        for(int i = 0; i < diff; ++i){
            fraction = fraction.substr(1, 24);
            fraction = fraction + '0';
            exp--; 
            if(exp < 0) {
                cout<<"UNDERFLOW"<<endl;
                cout<<"number of cycles: "<<cycles<<endl;
                return -1;
            }                
        }
        cycles++;
    }
    else{
        cycles += 2;
    }
    return 0;
}

bool is_normalised(string fraction){
    if(fraction[0] == '0' && fraction[1] == '1') return true;
    return false;
}

void adder(string input1, string input2){

    char sign1 = input1[0];
    string exp1 = input1.substr(1,8);
    string fraction1 = input1.substr(9, 32);

    char sign2 = input2[0];
    string exp2 = input2.substr(1,8);
    string fraction2 = input2.substr(9, 32);

    int cycles = 0;  

    //Step 1 Shifting exp = int_to_binary25()

    int shift_value = exponent_diff(exp1, exp2);

    bool shift_check = false;
    if(shift_value < 0) shift_check = true;
    
    if(shift_check)
        shift_fraction(fraction1, -1*shift_value);
    else 
        shift_fraction(fraction2, shift_value);
    
    cycles++;

    //Step 1 Shifting Done 

    //Step 2 Add Significand 

    string val = significand_sum(fraction1, sign1, fraction2, sign2, shift_value);
    
    string fraction = val.substr(1, 25);

    char sign = val[0];

    cycles++;

    //Step 2 Add Significand done

    bool next = true;

    string exp_res;

    while(next){
        int exp;

        if(shift_value == 0) 
            exp = binary_to_int(exp1);
        else if(shift_value < 0)
            exp = binary_to_int(exp2);
        else 
            exp = binary_to_int(exp1); 
        
        // deb(shift_value);
        // deb(exp);

        //step 3 normalise and step 4 rounding off in same function

        int res = 0;

        res = normalise(fraction, exp, cycles);

        if(res == -1) {
            return;
        }
       
        // cout<<"ok1\n";
        next = !is_normalised(fraction);
        // cout<<"ok2\n";
        exp_res = int_to_binary8(exp);
        // cout<<"ok3\n";
        //Step 3 and 4 done
    }
    
    string result = sign + exp_res + fraction.substr(2,23);   

    cout<<result<<endl;  
    cout<<"number of cycles: "<<cycles<<endl;
}

int check_NAN(string s){
    string hey = s.substr(1, 8);
    if(hey == "11111111"){
        if(s.substr(9, 23) == "00000000000000000000000"){
            if(hey[0] == '1') return -1;
            else if(hey[0] == '0') return 1;
        }
        else {
            return 2;
        }
    }
    return 0;
}

int check_zero(string s){
    if(s.substr(1, 31) == "0000000000000000000000000000000")
        return 1;
    else if(s.substr(1, 8) == "00000000") return 2;

    return 0;
}

int main(){
    ifstream infile;

    infile.open("test_11.txt");

    string line;

    while(getline(infile, line)){
        string input1 = line.substr(0,32);
        string input2 = line.substr(33,64);

        int one = check_NAN(input1);
        int two = check_NAN(input2);
        if(one == 0 && two == 0) {
            int num1 = check_zero(input1);
            int num2 = check_zero(input2);

            if(num1 == 2 || num2 == 2) cout<<"Denormalised input"<<endl;
            else if (num1 == 1 || num2 == 1){
                if(num1 == 1 && num2 != 1){
                    cout<<input2<<endl;
                    cout<<"no. of cycles: 4"<<endl;
                }
                else if(num2 == 1 && num1 != 1){
                    cout<<input1<<endl;
                    cout<<"no. of cycles: 4"<<endl;
                }
                else{
                    cout<<"Both inputs are zero"<<endl;
                }
            }

            adder(input1, input2);
        }
        else {
            if(one == 2 || two == 2) cout<<"NaN"<<endl;
            else if(one == 1 || two == 1 || one == -1 || two == -1)
                cout<<"Infinites in input. No computations"<<endl;
        }        
    }

    return 0;
}