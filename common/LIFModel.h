/**
 * @brief A leaky-integrate-and-fire (I&F) neural network model.
 *
 * @class LIFModel LIFModel.h "LIFModel.h"
 *
 * Implements both neuron and synapse behaviour.
 *
 * A standard leaky-integrate-and-fire neuron model is implemented
 * where the membrane potential \f$V_m\f$ of a neuron is given by
 * \f[
 *   \tau_m \frac{d V_m}{dt} = -(V_m-V_{resting}) + R_m \cdot (I_{syn}(t)+I_{inject}+I_{noise})
 * \f]
 * where \f$\tau_m=C_m\cdot R_m\f$ is the membrane time constant,
 * \f$R_m\f$ is the membrane resistance, \f$I_{syn}(t)\f$ is the
 * current supplied by the synapses, \f$I_{inject}\f$ is a
 * non-specific background current and \f$I_{noise}\f$ is a
 * Gaussian random variable with zero mean and a given variance
 * noise.
 *
 * At time \f$t=0\f$ \f$V_m\f$ is set to \f$V_{init}\f$. If
 * \f$V_m\f$ exceeds the threshold voltage \f$V_{thresh}\f$ it is
 * reset to \f$V_{reset}\f$ and hold there for the length
 * \f$T_{refract}\f$ of the absolute refractory period.
 *
 * The exponential Euler method is used for numerical integration.
 *
 * This model is a rewrite of work by Stiber, Kawasaki, Allan Ortiz, and Cory Mayberry
 *
 * @authors Derek McLean
 */
#pragma once
#ifndef _LIFMODEL_H_
#define _LIFMODEL_H_

#include "Model.h"
#include "Coordinate.h"

#include <vector>
#include <iostream>

using namespace std;

#define BYTES_OF_DELAYQUEUE         ( sizeof(uint32_t) / sizeof(uint8_t) )
#define LENGTH_OF_DELAYQUEUE        ( BYTES_OF_DELAYQUEUE * 8 )

class XmlRecorder;
#ifdef USE_HDF5
class Hdf5Recorder;
#endif // USE_HDF5
class SInputPoisson;

/**
 * Implementation of Model for the Leaky-Integrate-and-Fire model.
 */
class LIFModel : public Model, TiXmlVisitor
{
        friend XmlRecorder;
#ifdef USE_HDF5
        friend Hdf5Recorder;
#endif // USE_HDF5
	friend SInputPoisson;

    public:
        LIFModel();
        virtual ~LIFModel();

        /*
         * Declarations of concrete implementations of Model interface for an Leaky-Integrate-and-Fire
         * model.
         *
         * @see Model.h
         */

        bool readParameters(TiXmlElement *source);
        void printParameters(ostream &output) const;
        void loadMemory(istream& input, AllNeurons &neurons, AllSynapses &synapses, const SimulationInfo *sim_info);
        void saveMemory(ostream& output, AllNeurons &neurons, AllSynapses &synapses, const SimulationInfo *sim_info);
        void saveState(const AllNeurons &neurons, IRecorder* simRecorder);
        void createAllNeurons(AllNeurons &neurons, const SimulationInfo *sim_info);
        void setupSim(SimulationInfo *sim_info, const AllNeurons &neurons, AllSynapses &synapses, IRecorder* simRecorder);

    protected:

        /* -----------------------------------------------------------------------------------------
         * # Helper Functions
         * ------------------
         */

        // # Read Parameters
        // -----------------

        // Parse an element for parameter values.
        // Required by TiXmlVisitor, which is used by #readParameters
        bool VisitEnter(const TiXmlElement& element, const TiXmlAttribute* firstAttribute);

        // # Print Parameters
        // ------------------

        // Constructs a string representation of a specific neuron in the network.
        string neuronToString(AllNeurons& neurons, const int i) const;

        // # Load Memory
        // -------------

        // TODO
        bool updateDecay(AllSynapses &synapses, const int neuron_index, const int synapse_index, const BGFLOAT deltaT);

        // Deserialize a neuron from some input source.
        void readNeuron(istream &input, AllNeurons &neurons, const int index);
        // Deserialize a synapse from some input source.
        void readSynapse(istream &input, AllSynapses &synapses, const int neuron_index, const int synapse_index, const BGFLOAT deltaT);
        // TODO
        void initSpikeQueue(AllSynapses &synapses, const int neuron_index, const int synapse_index);
        // TODO
        void resetSynapse(AllSynapses &synapses, const int neuron_index, const int synapse_index, const BGFLOAT deltaT);
        // # Save Memory
        // -------------

        // Serialize a neuron to an output destination
        void writeNeuron(ostream& output, AllNeurons &neurons, const int index) const;
        // Serialize a synapse to an output destination
        void writeSynapse(ostream& output, AllSynapses &synapses, const int neuron_index, const int synapse_index) const;

        // # Save State
        // ------------
	void logSimStep(const AllNeurons &neurons, const AllSynapses &synapses, const SimulationInfo *sim_info) const;

        // TODO
        void getStarterNeuronMatrix(VectorMatrix& matrix, const bool* starter_map, const SimulationInfo *sim_info);

        // # Create All Neurons
        // --------------------

        // TODO
        void generateNeuronTypeMap(neuronType neuron_types[], int num_neurons);
        // TODO
        void initStarterMap(bool *starter_map, const int num_neurons, const neuronType neuron_type_map[]);
        // TODO
        void setNeuronDefaults(AllNeurons &neurons, const int index);
        // 
        void initNeuronConstsFromParamValues(AllNeurons &neurons, int neuron_index, const BGFLOAT deltaT);

        // # Update Connections
        // --------------------
                
	// TODO     
        void updateHistory(int currentStep, BGFLOAT epochDuration, AllNeurons &neurons, const SimulationInfo *sim_info, IRecorder* simRecorder);

	// TODO
	void clearSpikeCounts(AllNeurons &neurons, const SimulationInfo *sim_info);

        // TODO
        void updateFrontiers(const int num_neurons);
        // TODO     
        void updateOverlap(BGFLOAT num_neurons);


        // -----------------------------------------------------------------------------------------
        // # Generic Functions for handling synapse types
        // ---------------------------------------------

        // Converts the ordinal representation of a synapse type to its enum value.
        synapseType synapseOrdinalToType(const int type_ordinal);

        // TODO
        static const BGFLOAT SYNAPSE_STRENGTH_ADJUSTMENT;

	// CUDA needs these all to be visible... 
    //private:
        /** State of connections in the network. */
        struct Connections;

        // TODO
        struct GrowthParams
        {
            BGFLOAT epsilon; //null firing rate(zero outgrowth)
            BGFLOAT beta;  //sensitivity of outgrowth to firing rate
            BGFLOAT rho;  //outgrowth rate constant
            BGFLOAT targetRate; // Spikes/second
            BGFLOAT maxRate; // = targetRate / epsilon;
            BGFLOAT minRadius; // To ensure that even rapidly-firing neurons will connect to
                               // other neurons, when within their RFS.
            BGFLOAT startRadius; // No need to wait a long time before RFs start to overlap
        };

        // TODO
        friend ostream& operator<<(ostream &out, const GrowthParams &params);

        static const bool STARTER_FLAG; // = true; // true = use endogenously active neurons in simulation

        // TODO
        BGFLOAT m_Iinject[2];
        // TODO
        BGFLOAT m_Inoise[2];
        // TODO
        BGFLOAT m_Vthresh[2];
        // TODO
        BGFLOAT m_Vresting[2];
        // TODO
        BGFLOAT m_Vreset[2];
        // TODO
        BGFLOAT m_Vinit[2];
        // TODO
        BGFLOAT m_starter_Vthresh[2];
        // TODO
        BGFLOAT m_starter_Vreset[2];
        // TODO
        BGFLOAT m_new_targetRate;

        // Tracks the number of parameters that have been read by read params -
        // kind of a hack to do error handling for read params
        int m_read_params;

        //! True if a fixed layout has been provided
        bool m_fixed_layout;

        // TODO
        vector<int> m_endogenously_active_neuron_list;
        // TODO
        vector<int> m_inhibitory_neuron_layout;
        // TODO
        vector<int> m_probed_neuron_list;

        // TODO
        BGFLOAT m_frac_starter_neurons;
        // TODO
        BGFLOAT m_frac_excititory_neurons;

        // TODO
        GrowthParams m_growth;
        // TODO
        Connections *m_conns;

};


/**
 * Maintains intra-epoch state of connections in the network. This includes history and parameters
 * that inform how new connections are made during growth.
 */
struct LIFModel::Connections
{
        // TODO
        static const string MATRIX_TYPE;
        // TODO
        static const string MATRIX_INIT;

        // TODO
        int *spikeCounts;

        // TODO
        VectorMatrix xloc;
        // TODO
        VectorMatrix yloc;

        //! synapse weight
        CompleteMatrix W;
        //! neuron radii
        VectorMatrix radii;
        //! spiking rate
        VectorMatrix rates;
        //! Inter-neuron distance squared
        CompleteMatrix dist2;
        //! distance between connection frontiers
        CompleteMatrix delta;
        //! the true inter-neuron distance
        CompleteMatrix dist;
        //! areas of overlap
        CompleteMatrix area;
        //! neuron's outgrowth
        VectorMatrix outgrowth;
        //! displacement of neuron radii
        VectorMatrix deltaR;

        // TODO
        Connections(const int neuron_count, const BGFLOAT start_radius, const BGFLOAT growthEpochDuration, const BGFLOAT maxGrowthSteps);
};

#endif
