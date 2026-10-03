#include <iostream>  // For printing to the console
#include <fstream>   // For reading the Dataset.csv file
#include <sstream>   // For breaking each line into columns
#include <vector>    // For storing dynamic arrays of data
#include <string>    // For handling text data

using namespace std;

// Merges two halves of an array in ascending order
void merge(vector<double>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<double> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) { arr[k++] = L[i++]; } 
        else { arr[k++] = R[j++]; }
    }
    while (i < n1) { arr[k++] = L[i++]; }
    while (j < n2) { arr[k++] = R[j++]; }
}

// Sorts the array so we can find the exact middle value
void mergeSort(vector<double>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

double calculateMedian(vector<double> validValues) {
    if (validValues.empty()) return 0.0;
    
    // Sort the valid values manually
    mergeSort(validValues, 0, validValues.size() - 1);
    
    int n = validValues.size();
    if (n % 2 != 0) {
        return validValues[n / 2]; // Odd number of elements
    }
    // Even number of elements: average of the two middle values
    return (validValues[(n - 1) / 2] + validValues[n / 2]) / 2.0;
}

vector<vector<double>> parseAndCleanData(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file: " << filename << endl;
        return {};
    }

    string line, cell;
    vector<vector<string>> rawData;

    // 1. Read the entire CSV into a 2D string vector
    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> row;
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        rawData.push_back(row);
    }

    if (rawData.empty()) {
        return {};
    }

    int numCols = rawData[0].size();
    vector<vector<double>> cleanNumericData; 
    
    // 2. Process column by column
    for (int col = 0; col < numCols; ++col) {
        vector<double> validValues;
        int pureNumericCount = 0;
        int totalNonEmpty = 0;
        
        // Extract valid numbers to calculate the median
        for (int row = 1; row < rawData.size(); ++row) {
            string cell = rawData[row][col];
            
            if (!cell.empty() && cell.back() == '\r') cell.pop_back(); 
            
            // NEW: Treat explicit "NA" text as an empty missing value
            if (cell == "NA" || cell == "NaN") cell = ""; 

            if (!cell.empty()) {
                totalNonEmpty++;
                try {
                    if (cell[0] == '$') cell.erase(0, 1); 
                    
                    size_t pos;
                    double val = stod(cell, &pos); 
                    
                    if (pos == cell.length()) {
                        validValues.push_back(val);
                        pureNumericCount++;
                    }
                } catch (...) { 
                }
            }
        }
        
        // If less than 50% of the column is purely numbers, skip it
        if (totalNonEmpty == 0 || pureNumericCount < totalNonEmpty / 2) {
            continue; 
        }
        
        double median = calculateMedian(validValues);
        vector<double> currentCleanColumn;
        
        // 3. Build the final clean column, replacing missing values
        for (int row = 1; row < rawData.size(); ++row) {
            string cell = rawData[row][col];
            if (!cell.empty() && cell.back() == '\r') cell.pop_back(); 

            // NEW: Treat explicit "NA" text as an empty missing value
            if (cell == "NA" || cell == "NaN") cell = "";

            if (cell.empty()) {
                currentCleanColumn.push_back(median); 
            } else {
                try {
                    if (cell[0] == '$') cell.erase(0, 1);
                    currentCleanColumn.push_back(stod(cell));
                } catch (...) {
                    currentCleanColumn.push_back(median); 
                }
            }
        }
        
        cleanNumericData.push_back(currentCleanColumn);
    }

    return cleanNumericData;
}

int main() {
    const string filename = "Dataset.csv";
    cout << "Reading and cleaning " << filename << "..." << endl;

    // Load the dataset
    vector<vector<double>> numericAttributes = parseAndCleanData(filename);

    cout << "Data Preparation Complete!" << endl;
    cout << "Total numeric attributes found: " << numericAttributes.size() << endl;
    if (!numericAttributes.empty()) {
        cout << "Total entries per attribute: " << numericAttributes[0].size() << endl;
    }
    
    return 0;
}