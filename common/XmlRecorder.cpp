/**
 *      @file XmlRecorder.cpp
 *
 *      @brief An implementation for recording spikes history on xml file
 */
//! An implementation for recording spikes history on xml file

#include "XmlRecorder.h"
#include "AllLIFNeurons.h"      // TODO: remove LIF model specific code

//! THe constructor and destructor
XmlRecorder::XmlRecorder(IModel *model, SimulationInfo* sim_info) :
        burstinessHist("complete", "const", 1, static_cast<int>(sim_info->epochDuration * sim_info->maxSteps), 0),
        spikesHistory("complete", "const", 1, static_cast<int>(sim_info->epochDuration * sim_info->maxSteps * 100), 0),
        ratesHistory("complete", "const", static_cast<int>(sim_info->maxSteps + 1), sim_info->totalNeurons),
        radiiHistory("complete", "const", static_cast<int>(sim_info->maxSteps + 1), sim_info->totalNeurons),
        m_model(dynamic_cast<Model*> (model)),
        m_sim_info(sim_info)
{
}

XmlRecorder::~XmlRecorder()
{
}

/**
 * Initialize data
 * @param[in] stateOutputFileName	File name to save histories
 */
void XmlRecorder::init(const string& stateOutputFileName)
{
    stateOut.open( stateOutputFileName.c_str( ) );

}

/*
 * Init radii and rates history matrices with default values
 * @param[in] startRadius 	The starting connectivity radius for all neurons
 */
void XmlRecorder::initDefaultValues(BGFLOAT startRadius)
{
    for (int i = 0; i < m_sim_info->totalNeurons; i++)
    {
        radiiHistory(0, i) = startRadius;
        ratesHistory(0, i) = 0;
    }
}

/*
 * Init radii and rates history matrices with current radii and rates
 */
void XmlRecorder::initValues()
{
    for (int i = 0; i < m_sim_info->totalNeurons; i++)
    {
        radiiHistory(0, i) = (*m_model->getConnections()->radii)[i];
        ratesHistory(0, i) = (*m_model->getConnections()->rates)[i];
    }
}

/*
 * Get the current radii and rates values
 */
void XmlRecorder::getValues()
{
    for (int i = 0; i < m_sim_info->totalNeurons; i++)
    {
        (*m_model->getConnections()->radii)[i] = radiiHistory(m_sim_info->currentStep, i);
        (*m_model->getConnections()->rates)[i] = ratesHistory(m_sim_info->currentStep, i);
    }
}

/**
 * Terminate process
 */
void XmlRecorder::term()
{
    stateOut.close();
}

/**
 * Compile history information in every epoch
 * @param[in] neurons 	The entire list of neurons.
 * @param[in] minRadius	The minimum possible radius.
 */
void XmlRecorder::compileHistories(const AllNeurons &neurons, BGFLOAT minRadius)
{
    VectorMatrix& rates = (*m_model->getConnections()->rates);
    VectorMatrix& radii = (*m_model->getConnections()->radii);

    // output spikes
    for (int iNeuron = 0; iNeuron < m_sim_info->totalNeurons; iNeuron++)
    {
        uint64_t* pSpikes = neurons.spike_history[iNeuron];

        int& spike_count = neurons.spikeCount[iNeuron];
        for (int i = 0; i < spike_count; i++)
        {
            // compile network wide burstiness index data in 1s bins
            int idx1 = static_cast<int>( static_cast<double>( pSpikes[i] ) * m_sim_info->deltaT );
            burstinessHist[idx1] = burstinessHist[idx1] + 1.0;

            // compile network wide spike count in 10ms bins
            int idx2 = static_cast<int>( static_cast<double>( pSpikes[i] ) * m_sim_info->deltaT * 100 );
            spikesHistory[idx2] = spikesHistory[idx2] + 1.0;
        }

        // record firing rate to history matrix
        ratesHistory(m_sim_info->currentStep, iNeuron) = rates[iNeuron];

        // Cap minimum radius size and record radii to history matrix
        // TODO: find out why we cap this here.
        if (radii[iNeuron] < minRadius)
            radii[iNeuron] = minRadius;

        // record radius to history matrix
        radiiHistory(m_sim_info->currentStep, iNeuron) = radii[iNeuron];

        DEBUG_MID(cout << "radii[" << iNeuron << ":" << radii[iNeuron] << "]" << endl;)
    }
}

/**
 * Save current simulation state to XML
 * @param  neurons the Neuron list to search from.
 **/
void XmlRecorder::saveSimState(const AllNeurons &neurons)
{
    // create Neuron Types matrix
    VectorMatrix neuronTypes("complete", "const", 1, m_sim_info->totalNeurons, EXC);
    for (int i = 0; i < m_sim_info->totalNeurons; i++) {
        neuronTypes[i] = neurons.neuron_type_map[i];
    }

    // create neuron threshold matrix
    VectorMatrix neuronThresh("complete", "const", 1, m_sim_info->totalNeurons, 0);
    for (int i = 0; i < m_sim_info->totalNeurons; i++) {
        neuronThresh[i] = dynamic_cast<const AllLIFNeurons&>(neurons).Vthresh[i];
    }

    // neuron locations matrices
    VectorMatrix xloc("complete", "const", 1, m_sim_info->totalNeurons);
    VectorMatrix yloc("complete", "const", 1, m_sim_info->totalNeurons);

    // Initialize neurons
    for (int i = 0; i < m_sim_info->totalNeurons; i++)
    {
        xloc[i] = i % m_sim_info->width;
        yloc[i] = i / m_sim_info->width;
    }

    // Write XML header information:
    stateOut << "<?xml version=\"1.0\" standalone=\"no\"?>\n" << "<!-- State output file for the DCT growth modeling-->\n";
    //stateOut << version; TODO: version

    // Write the core state information:
    stateOut << "<SimState>\n";
    stateOut << "   " << radiiHistory.toXML("radiiHistory") << endl;
    stateOut << "   " << ratesHistory.toXML("ratesHistory") << endl;
    stateOut << "   " << burstinessHist.toXML("burstinessHist") << endl;
    stateOut << "   " << spikesHistory.toXML("spikesHistory") << endl;
    stateOut << "   " << xloc.toXML("xloc") << endl;
    stateOut << "   " << yloc.toXML("yloc") << endl;
    stateOut << "   " << neuronTypes.toXML("neuronTypes") << endl;

    // create starter nuerons matrix
    int num_starter_neurons = static_cast<int>(m_model->getLayout()->m_frac_starter_neurons * m_sim_info->totalNeurons);
    if (num_starter_neurons > 0)
    {
        VectorMatrix starterNeurons("complete", "const", 1, num_starter_neurons);
        getStarterNeuronMatrix(starterNeurons, neurons.starter_map, m_sim_info);
        stateOut << "   " << starterNeurons.toXML("starterNeurons") << endl;
    }

    // Write neuron thresold
    stateOut << "   " << neuronThresh.toXML("neuronThresh") << endl;

    // write time between growth cycles
    stateOut << "   <Matrix name=\"Tsim\" type=\"complete\" rows=\"1\" columns=\"1\" multiplier=\"1.0\">" << endl;
    stateOut << "   " << m_sim_info->epochDuration << endl;
    stateOut << "</Matrix>" << endl;

    // write simulation end time
    stateOut << "   <Matrix name=\"simulationEndTime\" type=\"complete\" rows=\"1\" columns=\"1\" multiplier=\"1.0\">" << endl;
    stateOut << "   " << g_simulationStep * m_sim_info->deltaT << endl;
    stateOut << "</Matrix>" << endl;
    stateOut << "</SimState>" << endl;
}

/**
 *  Get starter Neuron matrix.
 *  @param  matrix   Starter Neuron matrix.
 *  @param  starter_map bool map to reference neuron matrix location from.
 *  @param  sim_info    SimulationInfo class to read information from.
 */
void XmlRecorder::getStarterNeuronMatrix(VectorMatrix& matrix, const bool* starter_map, const SimulationInfo *sim_info)
{
    int cur = 0;
    for (int x = 0; x < sim_info->width; x++) {
        for (int y = 0; y < sim_info->height; y++) {
            if (starter_map[x + y * sim_info->width]) {
                matrix[cur] = x + y * sim_info->height;
                cur++;
            }
        }
    }
}
