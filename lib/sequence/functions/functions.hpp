#ifndef GENESIS_SEQUENCE_FUNCTIONS_FUNCTIONS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/functions/codes.hpp"
#include "utils/math/bitvector.hpp"

#include <iosfwd>
#include <map>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Sequence;
class SequenceSet;

// =================================================================================================
//     Accessors
// =================================================================================================

Sequence const* find_sequence( SequenceSet const& set, std::string const& label );

// =================================================================================================
//     Labels
// =================================================================================================

bool has_unique_labels( SequenceSet const& set, bool case_sensitive = true );

bool is_valid_label(   std::string const& label );
bool has_valid_label(  Sequence const&    seq );
bool has_valid_labels( SequenceSet const& set );

std::string sanitize_label( std::string const& label );
void sanitize_label( Sequence&     seq );
void sanitize_labels( SequenceSet& set );

// =================================================================================================
//     Characteristics
// =================================================================================================

std::map<char, size_t> site_histogram( Sequence    const& seq );
std::map<char, size_t> site_histogram( SequenceSet const& set );

std::map<char, double> base_frequencies( Sequence    const& seq, std::string const& plain_chars );
std::map<char, double> base_frequencies( SequenceSet const& set, std::string const& plain_chars );

size_t count_chars( SequenceSet const& set, std::string const& chars );

double gapyness( SequenceSet const& set, std::string const& gap_chars );

utils::Bitvector gap_sites(
    Sequence const& seq,
    std::string const& gap_chars = nucleic_acid_codes_undetermined()
);

utils::Bitvector gap_sites(
    SequenceSet const& set,
    std::string const& gap_chars = nucleic_acid_codes_undetermined()
);

bool validate_chars( SequenceSet const& set, std::string const& chars );

size_t total_length( SequenceSet const& set );

bool is_alignment( SequenceSet const& set );

// =================================================================================================
//     Modifiers
// =================================================================================================

void remove_sites( Sequence&    seq, utils::Bitvector sites );
void remove_sites( SequenceSet& set, utils::Bitvector sites );

void replace_characters( Sequence&    seq, std::string const& search, char replacement );
void replace_characters( SequenceSet& set, std::string const& search, char replacement );

void replace_u_with_t( Sequence&    seq );
void replace_u_with_t( SequenceSet& set );
void replace_t_with_u( Sequence&    seq );
void replace_t_with_u( SequenceSet& set );

// =================================================================================================
//     Filters
// =================================================================================================

void filter_min_sequence_length( SequenceSet& set, size_t min_length );

// =================================================================================================
//     Print and Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, Sequence    const& seq );
std::ostream& operator << ( std::ostream& out, SequenceSet const& set );

std::string print(
    Sequence const&                    seq,
    bool                               print_label  = true,
    size_t                             length_limit = 100
);

std::string print(
    SequenceSet const&                 set,
    bool                               print_label    = true,
    size_t                             length_limit   = 100,
    size_t                             sequence_limit = 10
);

std::string print_color(
    Sequence const&                    seq,
    std::map<char, std::string> const& colors,
    bool                               print_label  = true,
    size_t                             length_limit = 100,
    bool                               background   = true
);

std::string print_color(
    SequenceSet const&                 set,
    std::map<char, std::string> const& colors,
    bool                               print_label    = true,
    size_t                             length_limit   = 100,
    size_t                             sequence_limit = 10,
    bool                               background     = true
);

} // namespace sequence
} // namespace genesis

#endif // include guard
