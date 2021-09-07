/***********************************************************/
/***      ______  ____  ______                 _         ***/
/***     / ___\ \/ /\ \/ / ___|_ __ __ _ _ __ | |__	     ***/
/***    | |    \  /  \  / |  _| '__/ _` | '_ \| '_ \	 ***/
/***    | |___ /  \  /  \ |_| | | | (_| | |_) | | | |    ***/
/***     \____/_/\_\/_/\_\____|_|  \__,_| .__/|_| |_|    ***/
/***                                    |_|			     ***/
/***********************************************************/
/***     Header-Only C++ Library for Graph			     ***/
/***	 Representation and Algorithms				     ***/
/***********************************************************/
/***     Author: ZigRazor			     			     ***/
/***	 E-Mail: zigrazor@gmail.com 				     ***/
/***********************************************************/
/***	 Collaboration: ----------- 				     ***/
/***********************************************************/
/***	 License: AGPL v3.0							     ***/
/***********************************************************/

#ifndef __CXXGRAPH_PARTITIONING_PARTITIONER_H__
#define __CXXGRAPH_PARTITIONING_PARTITIONER_H__

#pragma once
#include <vector>
#include "PartitionStrategy.hpp"
#include "Partitioning/Utility/Globals.hpp"
#include "Edge/Edge.hpp"
#include "CoordinatedPartitionState.hpp"

namespace CXXGRAPH
{
    namespace PARTITIONING
    {
        template <typename T>
        class Partitioner
        {
        private:
            std::vector<Edge<T>> dataset;
            PartitionStrategy<T> algorithm;
            Globals GLOBALS;

            CoordinatedPartitionState<T> startCoordinated();

        public:
            Partitioner(std::vector<Edge<T>> &dataset, Globals &G);
            ~Partitioner();

            CoordinatedPartitionState<T> performCoordinatedPartition();
        };
        template <typename T>
        Partitioner<T>::Partitioner(std::vector<Edge<T>> &dataset, Globals &G)
        {
            this->GLOBALS = G;
            this->dataset = dataset;
            if (GLOBALS.partitionStategy == CXXGRAPH::PartitionAlgorithm::HDRF)
            {
                algorithm = new HDRF<T>(GLOBALS);
            }
        }
        template <typename T>
        CoordinatedPartitionState<T> Partitioner<T>::startCoordinated()
        {
            CoordinatedPartitionState state(GLOBALS);
            int processors = GLOBALS.threads;

            std::thread myThreads[processors];

            int n = dataset.size();
            int subSize = n / processors + 1;
            for (int t = 0; t < processors; t++)
            {
                int iStart = t * subSize;
                int iEnd = std::min((t + 1) * subSize, n);
                if (iEnd >= iStart)
                {
                    std::vector<Edge<T>> list(dataset.begin() + iStart, dataset.begin() + iEnd);
                    Runnable x = PartitionerThread<T>(list, state, algorithm, new std::list<int>());
                    myThreads[t] = std::thread(&Runnable::run, &x);
                }
            }
            for (int t = 0; t < processors; t++)
            {
                myThreads[t].join();
            }
            return state;
        }
        template <typename T>
        Partitioner<T>::~Partitioner()
        {
        }
        template <typename T>
        CoordinatedPartitionState<T> Partitioner<T>::performCoordinatedPartition()
        {
            return startCoordinated();
        }
    }
}

#endif // __CXXGRAPH_PARTITIONING_PARTITIONER_H__