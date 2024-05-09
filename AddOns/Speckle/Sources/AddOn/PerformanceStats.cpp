#include "PerformanceStats.h"

PerformanceStats::PerformanceStats() : exportTime(0.0), propertyReadTime(0.0), jsonBuildTime(0.0), fileIOTime(0.0) {}

PerformanceStats& PerformanceStats::GetInstance() 
{
    static PerformanceStats instance;
    return instance;
}

void PerformanceStats::IncreaseExportTime(double value)
{
    exportTime += value;
}

void PerformanceStats::IncreasePropertyReadTime(double value)
{
    propertyReadTime += value;
}

void PerformanceStats::IncreasePropertyListBuildTime(double value)
{
    propertyListBuildTime += value;
}

void PerformanceStats::IncreaseJsonBuildTime(double value)
{
    jsonBuildTime += value;
}

void PerformanceStats::IncreaseJsonPushBackTime(double value)
{
    jsonPushBackTime += value;
}



void PerformanceStats::IncreaseFileIOTime(double value)
{
    fileIOTime += value;
}

void PerformanceStats::PrintStats() const 
{
    std::string filename = "C:/tmp/stats.txt";
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << "Export Time: " << exportTime << std::endl;
        outFile << "Property Read Time: " << propertyReadTime << std::endl;
        outFile << "Property List Build Time: " << propertyListBuildTime << std::endl;
        outFile << "JSON Build Time: " << jsonBuildTime << std::endl;
        outFile << "JSON push_back Time: " << jsonPushBackTime << std::endl;
        outFile << "File IO Time: " << fileIOTime << std::endl;
        outFile.close();
        std::cout << "Statistics written to " << filename << std::endl;
    }
    else {
        std::cerr << "Unable to open file " << filename << " for writing." << std::endl;
    }
}

void PerformanceStats::Clear()
{
    exportTime = 0.0;
    propertyReadTime = 0.0;
    jsonBuildTime = 0.0;
    fileIOTime = 0.0;
}
