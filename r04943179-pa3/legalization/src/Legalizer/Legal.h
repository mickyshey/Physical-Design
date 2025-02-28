#ifndef _CLEGAL_
#define _CLEGAL_

#include "Util.h"
#include "Row.h"
#include "Placement.h"
#include "GnuplotLivePlotter.h"
#include "GnuplotMatrixPlotter.h"
#include <math.h>
#include <stdio.h>

using namespace std;

class MNode;
class Cluster;

class CLegal
{
    public:
        CLegal( Placement& placement );
       ~CLegal(void){}

        Placement& _placement;
        GnuplotLivePlotter gnuplotLivePlotter;

    //Solve the legalization, "true" indicates success
        bool solve();
    //Check the legalization result is overlap-free, within boundary, and on-site, "true" indicates legal
        bool check();
        void saveGlobalResult();
        void setLegalResult();
        double totalDisplacement();

        //**********************************
        //* Information of Macros or Cells *
        //**********************************

        vector<unsigned> m_cell_order; //Used as legalization order
        vector<Row> m_free_sites; //Record the unused sites
        vector<Row> m_left_free_sites;

        vector<CPoint> m_origLocations;	//Record the original locations
        vector<CPoint> m_bestLocations;	//Record the best locations
        vector<Row> m_best_sites;       //Record the resulting sites of the best result

        vector<CPoint> m_bestLocations_left;
        vector<Row> m_best_sites_left;
        vector<CPoint> m_globalLocations;

        vector<int> m_macro_ids;
        vector<double> m_orig_widths;


        //Variables and functions used for 4-directional macro shifter
        vector<CPoint> m_macro_shifter_orig_positions;	//Record the original macro positions (left-bottom)
                                                        //before macro shifter
        vector<CPoint> m_macro_shifter_best_positions;	//Record the best macro positions (left-bottom)
                                                        //in 4-directional macro shifter

        vector<int> m_process_list;    //CalculateCellOrder() and CalculateNewLocation()
                                       //only act on modules in the m_process_list
        //********************************
        //*           parameter          *
        //********************************
        double m_max_module_height;
        double m_max_module_width;
        double m_average_cell_width;    //Average standard cell width
        double m_site_bottom, m_site_height; //Keep the bottom y coordinate of the sites and the
                                                                   //height of each site
        int m_unlegal_count;            //Number of unlegalized cells
        double m_chip_left_bound;	//Record the chip left boundary
		double m_chip_right_bound;	//Record the chip right boundary

        //*******************************
        //*		defined by r04943179	*
        //*******************************
		double moduleWidth(unsigned moduleIdx) { return _placement.module(moduleIdx).width(); }
		void InitCellOrder();
		void recordOrigInfo(vector<Cluster*>& clusters, vector< vector<double> >& origInfo);
		void recoverFromOrigInfo(vector<Cluster*>& clusters, vector< vector<double> >& origInfo, unsigned mergeCount);
		void recordCurrLocations(vector<Cluster*>& clusters);
		void recoverCurrLocations(vector<Cluster*>& clusters, unsigned mergeCount);
		unsigned placeRow(unsigned moduleIdx, vector<Cluster*>& clusters, vector< vector<double> >& origInfo, bool isReal);
		void collapse(unsigned clusterIdx, vector<Cluster*>& clusters, bool isReal, unsigned& mergeCount, vector< vector<double> >& origInfo);
		void merge(unsigned clusterIdx, vector<Cluster*>& clusters, bool isReal, vector< vector<double> >& origInfo);
		void updateX(unsigned mergeCount, vector<Cluster*>& clusters);
		void undoPlaceRow(unsigned mergeCount, vector<Cluster*>& clusters, vector< vector<double> >& origInfo);
		double getCost(vector<Cluster*>& clusters, unsigned mergeCount);
		bool overUpperBound(unsigned moduleIdx, unsigned rowIdx, double bestCost);
		void set2BestLocations(vector<Cluster*>& clusters, unsigned rowIdx, unsigned mergeCount);

		bool fitInCore(vector<Cluster*>& clusters, unsigned mergeCount);
		void mergeClustersReal(vector<Cluster*>& clusters);

		void report();
		void reportClusters(vector<Cluster*>& clusters);
		void reportCurrLocations(vector<Cluster*>& clusters);
		void reportOrigInfo(vector< vector<double> >& origInfo);
	
		vector< vector<Cluster*> > _clusters_in_row; 
		vector<double>					_rowWidth;

};

class Cluster
{
	
	public:
		Cluster():
			_X(0),
			_E(0),
			_Q(0),
			_W(0) { _cells.clear(); }
		Cluster(unsigned moduleIdx) {
			_cells.clear();
			_cells.push_back(moduleIdx);
			_X = _legal -> m_globalLocations[moduleIdx].x;
			_E = 1;
			_Q = _legal -> m_globalLocations[moduleIdx].x;	// _E = 1, _W = 0
			_W = _legal -> moduleWidth(moduleIdx); }	
		Cluster(Cluster* c):
			_X(c -> getX()),
			_E(c -> getE()),
			_Q(c -> getQ()),
			_W(c -> getW()) { _cells.clear(); }
		~Cluster() { _cells.clear(); }

		//void reset() { _origX = _origE = _origQ = _origW = 0; }

		void addCell(unsigned moduleIdx, bool isReal);
		void set2BestLocations(unsigned rowIdx);

		double getX() const { return _X; }
		double getE() const { return _E; }
		double getQ() const { return _Q; }
		double getW() const { return _W; }
		unsigned size() const { return _cells.size(); }
		void pushBack(unsigned moduleIdx) { _cells.push_back(moduleIdx); }
		void popBack() { _cells.pop_back(); }
		unsigned getModuleIdx(unsigned i) { return _cells[i]; }

		void setX(double x) { _X = x; }
		void setE(double e) { _E = e; }
		void setQ(double q) { _Q = q; }
		void setW(double w) { _W = w; }

		static void setCLegal(CLegal* c) { _legal = c; }

	private:	
		// assume all modules are weighted 1
		double _X;
		double _E;
		double _Q;
		double _W;
		vector<unsigned> _cells;

		static CLegal* _legal;
};

#endif 

