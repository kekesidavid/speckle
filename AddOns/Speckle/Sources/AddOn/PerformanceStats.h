#pragma once

#include <iostream>
#include <fstream>

class PerformanceStats {
private:
    double exportTime;
    double propertyReadTime;
    double propertyListBuildTime;
    double jsonBuildTime;
    double jsonPushBackTime;
    double fileIOTime;

    PerformanceStats();

public:
    static PerformanceStats& GetInstance();

    void IncreaseExportTime(double value);
    void IncreasePropertyReadTime(double value);
    void IncreasePropertyListBuildTime(double value);
    void IncreaseJsonBuildTime(double value);
    void IncreaseJsonPushBackTime(double value);
    void IncreaseFileIOTime(double value);

    void PrintStats() const;
    void Clear();
};
