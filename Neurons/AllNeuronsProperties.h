/**
 *      @file AllNeuronsProperties.h
 *
 *      @brief A container of the base class of all neuron data
 */

#pragma once

#include "IAllNeuronsProperties.h"

class AllNeuronsProperties : public IAllNeuronsProperties
{
    public:
        AllNeuronsProperties();
        virtual ~AllNeuronsProperties();

        /**
         *  Setup the internal structure of the class.
         *  Allocate memories to store all neurons' state.
         *
         *  @param  sim_info  SimulationInfo class to read information from.
         *  @param  clr_info  ClusterInfo class to read information from.
         */
        virtual void setupNeuronsProperties(SimulationInfo *sim_info, ClusterInfo *clr_info);

    private:
        /**
         *  Cleanup the class.
         *  Deallocate memories.
         */
        void cleanupNeuronsProperties();

    protected:
        /**
         *  Total number of neurons.
         */
        int size;

        /**
         *  Number of parameters read.
         */
        int nParams;

    public:
        /**
         *  The summation point for each neuron.
         *  Summation points are places where the synapses connected to the neuron
         *  apply (summed up) their PSRs (Post-Synaptic-Response).
         *  On the next advance cycle, neurons add the values stored in their corresponding
         *  summation points to their Vm and resets the summation points to zero
         */
        BGFLOAT *summation_map;
};