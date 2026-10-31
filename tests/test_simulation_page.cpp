/** Checks that the simulation page is generated with the data inlined. */
#include "SimulationPage.h"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace attitude;

namespace {
int failures = 0;
void expect(bool ok, const std::string& what) {
    std::cout << (ok ? "  ok   " : "  FAIL ") << what << "\n";
    if (!ok) ++failures;
}
} // namespace

int main() {
    const std::vector<AccelRecord> samples = {{100, {27, -22, -982}}, {110, {23, -22, -993}}};
    const std::vector<EstimationRecord> estimates = {{100, {1.2829, -178.7166}}, {110, {1.2688, -178.7308}}};

    const std::string html = SimulationPage::render(samples, estimates, "AN3461 eq. 25/26", "attitude_exam.log");

    expect(html.find("<!DOCTYPE html>") == 0, "starts with an HTML document");
    expect(html.find("__DATA__") == std::string::npos, "data placeholder replaced");
    expect(html.find("__EQUATIONS__") == std::string::npos, "equations placeholder replaced");
    expect(html.find("__SOURCE_NAME__") == std::string::npos, "source placeholder replaced");
    expect(html.find("[100,27.0000,-22.0000,-982.0000,1.2829,-178.7166]") != std::string::npos, "first row inlined");
    expect(html.find("const DATA = [[100,") != std::string::npos, "rows assigned to DATA");
    expect(html.find("equations: 'AN3461 eq. 25/26'") != std::string::npos, "equations name inlined");
    expect(html.find("source: 'attitude_exam.log'") != std::string::npos, "source name inlined");

    bool threw = false;
    try { SimulationPage::render(samples, {}, "x", "y"); } catch (const std::invalid_argument&) { threw = true; }
    expect(threw, "rejects mismatched sample/estimate lengths");

    if (failures) { std::cerr << failures << " check(s) failed\n"; return EXIT_FAILURE; }
    std::cout << "All checks passed\n";
    return EXIT_SUCCESS;
}
