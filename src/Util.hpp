// Copyright (c) 2012-2017 FRC Team 3512, Spartatroniks. All Rights Reserved.

#pragma once

#include <string>
#include <vector>

// Contains miscellaneous utility functions

template <class T>
bool packetToVar(const std::vector<char>& data, size_t& pos, T& dest);

bool packetToVar(const std::vector<char>& data, size_t& pos, std::string& dest);

#include "Util.inl"
