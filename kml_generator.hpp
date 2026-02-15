// kml_generator.hpp
// Add this file to your project directory (same folder as graph.hpp)

#ifndef KML_GENERATOR_HPP
#define KML_GENERATOR_HPP

#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// Simple KML writer - just outputs the path
void writeSimpleKML(
    const string& filename,
    const vector<pair<double, double>>& coordinates,
    const string& title = "Route",
    const string& description = "Optimal route"
)
{
    ofstream kml(filename);
    
    kml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    kml << "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n";
    kml << "<Document>\n";
    kml << "  <Placemark>\n";
    kml << "    <name>" << title << "</name>\n";
    kml << "    <description>" << description << "</description>\n";
    kml << "    <LineString>\n";
    kml << "      <tessellate>1</tessellate>\n";
    kml << "      <coordinates>\n";
    
    for(const auto& coord : coordinates) {
        kml << "        " << fixed << setprecision(6)
            << coord.first << "," << coord.second << ",0\n";
    }
    
    kml << "      </coordinates>\n";
    kml << "    </LineString>\n";
    kml << "  </Placemark>\n";
    kml << "</Document>\n";
    kml << "</kml>\n";
    
    kml.close();
}

// Advanced KML writer - with colors and markers
void writeAdvancedKML(
    const string& filename,
    const vector<pair<double, double>>& pathCoords,
    const vector<string>& segmentModes,  // "Car", "Metro", "Bus", etc.
    const pair<double, double>& sourceCoord,
    const pair<double, double>& destCoord,
    const string& title,
    const string& summary
)
{
    ofstream kml(filename);
    
    kml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    kml << "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n";
    kml << "<Document>\n";
    kml << "  <name>" << title << "</name>\n";
    kml << "  <description>" << summary << "</description>\n";
    
    // Define styles
    kml << "  <Style id=\"carStyle\">\n";
    kml << "    <LineStyle><color>ff0000ff</color><width>4</width></LineStyle>\n";
    kml << "  </Style>\n";
    
    kml << "  <Style id=\"metroStyle\">\n";
    kml << "    <LineStyle><color>ff00ff00</color><width>4</width></LineStyle>\n";
    kml << "  </Style>\n";
    
    kml << "  <Style id=\"bikolpoBusStyle\">\n";
    kml << "    <LineStyle><color>ffff0000</color><width>3</width></LineStyle>\n";
    kml << "  </Style>\n";
    
    kml << "  <Style id=\"uttaraBusStyle\">\n";
    kml << "    <LineStyle><color>ffff8800</color><width>3</width></LineStyle>\n";
    kml << "  </Style>\n";
    
    kml << "  <Style id=\"walkStyle\">\n";
    kml << "    <LineStyle><color>ff888888</color><width>2</width></LineStyle>\n";
    kml << "  </Style>\n";
    
    kml << "  <Style id=\"sourceStyle\">\n";
    kml << "    <IconStyle><color>ff00ff00</color></IconStyle>\n";
    kml << "  </Style>\n";
    
    kml << "  <Style id=\"destStyle\">\n";
    kml << "    <IconStyle><color>ff0000ff</color></IconStyle>\n";
    kml << "  </Style>\n";
    
    // Add source marker
    kml << "  <Placemark>\n";
    kml << "    <name>Source</name>\n";
    kml << "    <description>Starting point</description>\n";
    kml << "    <styleUrl>#sourceStyle</styleUrl>\n";
    kml << "    <Point>\n";
    kml << "      <coordinates>" << fixed << setprecision(6)
        << sourceCoord.first << "," << sourceCoord.second << ",0</coordinates>\n";
    kml << "    </Point>\n";
    kml << "  </Placemark>\n";
    
    // Add destination marker
    kml << "  <Placemark>\n";
    kml << "    <name>Destination</name>\n";
    kml << "    <description>Ending point</description>\n";
    kml << "    <styleUrl>#destStyle</styleUrl>\n";
    kml << "    <Point>\n";
    kml << "      <coordinates>" << fixed << setprecision(6)
        << destCoord.first << "," << destCoord.second << ",0</coordinates>\n";
    kml << "    </Point>\n";
    kml << "  </Placemark>\n";
    
    // Add path segments
    for(size_t i = 0; i + 1 < pathCoords.size(); i++) {
        string mode = (i < segmentModes.size()) ? segmentModes[i] : "Unknown";
        string styleUrl = "#carStyle"; // default
        
        if(mode == "Metro") styleUrl = "#metroStyle";
        else if(mode == "BikolpoBus") styleUrl = "#bikolpoBusStyle";
        else if(mode == "UttaraBus") styleUrl = "#uttaraBusStyle";
        else if(mode == "Walk") styleUrl = "#walkStyle";
        
        kml << "  <Placemark>\n";
        kml << "    <name>Segment " << (i+1) << " - " << mode << "</name>\n";
        kml << "    <styleUrl>" << styleUrl << "</styleUrl>\n";
        kml << "    <LineString>\n";
        kml << "      <tessellate>1</tessellate>\n";
        kml << "      <coordinates>\n";
        kml << "        " << fixed << setprecision(6)
            << pathCoords[i].first << "," << pathCoords[i].second << ",0\n";
        kml << "        " << pathCoords[i+1].first << "," << pathCoords[i+1].second << ",0\n";
        kml << "      </coordinates>\n";
        kml << "    </LineString>\n";
        kml << "  </Placemark>\n";
    }
    
    kml << "</Document>\n";
    kml << "</kml>\n";
    
    kml.close();
}

#endif // KML_GENERATOR_HPP