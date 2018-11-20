#ifndef GENESIS_UTILS_MATH_STATISTICS_H_
#define GENESIS_UTILS_MATH_STATISTICS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @ingroup utils
 */

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/ranking.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Structures and Classes
// =================================================================================================

/**
 * @brief Store a pair of min and max values.
 *
 * This notation is simply more readable than the std default of using a `pair<T, T>`.
 */
template< typename T >
struct MinMaxPair
{
    T min;
    T max;
};

/**
 * @brief Store a mean and a standard deviation value.
 *
 * This notation is simply more readable than the std default of using a `pair<T, T>` for such
 * types.
 */
struct MeanStddevPair
{
    double mean;
    double stddev;
};

/**
 * @brief Store the values of quartiles: `q0 == min`, `q1 == 25%`, `q2 == 50%`, `q3 == 75%`,
 * `q4 == max`.
 */
struct Quartiles
{
    double q0 = 0.0;
    double q1 = 0.0;
    double q2 = 0.0;
    double q3 = 0.0;
    double q4 = 0.0;
};

// =================================================================================================
//     Normalization
// =================================================================================================

/**
 * @brief Calculate the closure of a range of numbers.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element. Then, the closure [1] of the elements is calculated,
 * that is, they are all divided by their total sum. This is used in compositional data analysis.
 * Non-finite elements are ignored, negative elements throw an exception.
 *
 * > [1] J. Aitchison,
 * > "The statistical analysis of compositional data".
 * > Chapman and Hall London, 1986.
 * > https://www.jstor.org/stable/2345821
 */
template <class ForwardIterator>
void closure( ForwardIterator first, ForwardIterator last )
{
    // Prepare result.
    double sum = 0.0;
    size_t cnt = 0;

    // Sum up elements.
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            if( *it < 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate closure of negative numbers."
                );
            }

            sum += *it;
            ++cnt;
        }
        ++it;
    }

    // If there are no valid elements, return.
    if( cnt == 0 ) {
        return;
    }

    // Make the closure.
    it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            *it /= sum;
        }
        ++it;
    }
}

/**
 * @brief Calculate the closure of a `std::vector` of `double` elements.
 *
 * @see closure( ForwardIterator first, ForwardIterator last ) for details.
 */
inline void closure( std::vector<double>& vec )
{
    return closure( vec.begin(), vec.end() );
}

// =================================================================================================
//     Mean Stddev
// =================================================================================================

/**
 * @brief Calculate the arithmetic mean and standard deviation of a range of `double` elements.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element.
 * The function then calculates the arithmetic mean and standard deviation of all finite elements
 * in the range. If no elements are finite, or if the range is empty, both returned values are `0.0`.
 * Non-finite numbers are ignored.
 *
 * If the resulting standard deviation is below the given @p epsilon (e.g, `0.0000001`), it is
 * "corrected" to be `1.0` instead. This is an inelegant (but usual) way to handle near-zero values,
 * which for some use cases would cause problems like a division by zero later on.
 * By default, @p epsilon is `-1.0`, which deactivates this check - a standard deviation can never
 * be below `0.0`.
 *
 * @see mean_stddev( std::vector<double> const&, double epsilon ) for a version for `std::vector`.
 * @see arithmetic_mean() for a function that only calculates the mean, and thus saves the effort
 * of a second iteration over the range.
 */
template <class ForwardIterator>
MeanStddevPair mean_stddev( ForwardIterator first, ForwardIterator last, double epsilon = -1.0 )
{
    // Prepare result.
    MeanStddevPair result;
    result.mean   = 0.0;
    result.stddev = 0.0;
    size_t count  = 0;

    // Sum up elements.
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            result.mean += *it;
            ++count;
        }
        ++it;
    }

    // If there are no valid elements, return an all-zero result.
    if( count == 0 ) {
        return result;
    }

    //  Calculate mean.
    result.mean /= static_cast<double>( count );

    // Calculate std dev.
    it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            result.stddev += (( *it - result.mean ) * ( *it - result.mean ));
        }
        ++it;
    }
    assert( count > 0 );
    result.stddev /= static_cast<double>( count );
    result.stddev = std::sqrt( result.stddev );

    // The following in an inelegant (but usual) way to handle near-zero values,
    // which later would cause a division by zero.
    assert( result.stddev >= 0.0 );
    if( result.stddev <= epsilon ){
        result.stddev = 1.0;
    }

    return result;
}

/**
 * @brief Calculate the mean and standard deviation of a `std::vector` of `double` elements.
 *
 * @see mean_stddev( ForwardIterator first, ForwardIterator last, double epsilon ) for details.
 * @see arithmetic_mean() for a function that only calculates the mean, and thus saves the effort
 * of a second iteration over the range.
 */
inline MeanStddevPair mean_stddev( std::vector<double> const& vec, double epsilon = -1.0 )
{
    return mean_stddev( vec.begin(), vec.end(), epsilon );
}

// =================================================================================================
//     Arithmetic Mean
// =================================================================================================

/**
 * @brief Calculate the arithmetic mean of a range of numbers.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element.
 * The function then calculates the arithmetic mean of all finite elements in the range.
 * If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored.
 *
 * @see arithmetic_mean( std::vector<double> const& ) for a version for `std::vector`.
 * @see mean_stddev() for a function that also calculates the standard deviation.
 * @see geometric_mean() for a function that calculates the geometric mean.
 * @see weighted_arithmetic_mean() for a function that calculates the weighted arithmetic mean.
 */
template <class ForwardIterator>
double arithmetic_mean( ForwardIterator first, ForwardIterator last )
{
    // Prepare result.
    double mean  = 0.0;
    size_t count = 0;

    // Sum up elements.
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            mean += *it;
            ++count;
        }
        ++it;
    }

    // If there are no valid elements, return an all-zero result.
    if( count == 0 ) {
        assert( mean == 0.0 );
        return mean;
    }

    //  Calculate mean.
    assert( count > 0 );
    return mean / static_cast<double>( count );
}

/**
 * @brief Calculate the arithmetic mean of a `std::vector` of `double` elements.
 *
 * @see arithmetic_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see mean_stddev() for a function that simultaneously calculates the standard deviation.
 * @see geometric_mean() for a function that calculates the geometric mean.
 */
inline double arithmetic_mean( std::vector<double> const& vec )
{
    return arithmetic_mean( vec.begin(), vec.end() );
}

/**
 * @brief Calculate the weighted arithmetic mean of a range of `double` values.
 *
 * The iterators @p first_value and @p last_value, as well as @p first_weight and @p last_weight,
 * need to point to ranges of `double` values, with @p last_value and @p last_weight being the
 * past-the-end elements. Both ranges need to have the same size.
 * The function then calculates the weighted arithmetic mean of all finite elements
 * in the range. If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored. The weights have to be non-negative.
 *
 * @see weighted_arithmetic_mean( std::vector<double> const& ) for a version for `std::vector`.
 * @see arithmetic_mean() for the unweighted version.
 * @see geometric_mean() for a function that calculates the geometric mean.
 * @see weighted_geometric_mean() for a function that calculates the weighted geometric mean.
 */
template <class ForwardIterator>
double weighted_arithmetic_mean(
    ForwardIterator first_value,  ForwardIterator last_value,
    ForwardIterator first_weight, ForwardIterator last_weight
) {
    double num = 0.0;
    double den = 0.0;
    size_t cnt = 0;

    // Multiply elements.
    for_each_finite_pair( first_value, last_value, first_weight, last_weight, [&]( double value, double weight ){
        if( weight < 0.0 ) {
            throw std::invalid_argument(
                "Cannot calculate weighted arithmetic mean with negative weights."
            );
        }

        num += weight * value;
        den += weight;
        ++cnt;
    });

    // If there are no valid elements, return an all-zero result.
    if( cnt == 0 ) {
        return 0.0;
    }
    if( den == 0.0 ) {
        throw std::invalid_argument(
            "Cannot calculate weighted arithmetic mean with all weights being 0."
        );
    }

    // Return the result.
    assert( cnt > 0 );
    assert( den > 0.0 );
    return ( num / den );
}

/**
 * @brief Calculate the weighted arithmetic mean of a `std::vector` of `double` elements.
 *
 * @see weighted_arithmetic_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see arithmetic_mean() for the unweighted version.
 * @see geometric_mean() for a function that calculates the geometric mean.
 * @see weighted_geometric_mean() for a function that calculates the weighted geometric mean.
 */
inline double weighted_arithmetic_mean(
    std::vector<double> const& values,
    std::vector<double> const& weights
) {
    return weighted_arithmetic_mean( values.begin(), values.end(), weights.begin(), weights.end() );
}

// =================================================================================================
//     Geometric Mean
// =================================================================================================

/**
 * @brief Calculate the geometric mean of a range of positive numbers.
 *
 * The iterators @p first and @p last need to point to a range of `double` values,
 * with @p last being the past-the-end element.
 * The function then calculates the geometric mean of all positive finite elements in the range.
 * If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored. If finite non-positive numbers (zero or negative) are found,
 * an exception is thrown.
 *
 * @see geometric_mean( std::vector<double> const& ) for a version for `std::vector`.
 * @see weighted_geometric_mean() for a weighted version.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean.
 */
template <class ForwardIterator>
double geometric_mean( ForwardIterator first, ForwardIterator last )
{
    double prod  = 1.0;
    size_t count = 0;

    // Multiply elements.
    auto it = first;
    while( it != last ) {
        if( std::isfinite( *it ) ) {
            if( *it <= 0.0 ) {
                throw std::invalid_argument(
                    "Cannot calculate geometric mean of non-positive numbers."
                );
            }
            prod *= *it;
            ++count;
        }
        ++it;
    }

    // If there are no valid elements, return an all-zero result.
    if( count == 0 ) {
        return 0.0;
    }

    // Return the result.
    assert( prod  > 0.0 );
    assert( count > 0 );
    return std::pow( prod, 1.0 / static_cast<double>( count ));
}

/**
 * @brief Calculate the geometric mean of a `std::vector` of `double` elements.
 *
 * @see geometric_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean.
 */
inline double geometric_mean( std::vector<double> const& vec )
{
    return geometric_mean( vec.begin(), vec.end() );
}

/**
 * @brief Calculate the weighted geometric mean of a range of positive numbers.
 *
 * The iterators @p first_value and @p last_value, as well as @p first_weight and @p last_weight,
 * need to point to ranges of `double` values, with @p last_value and @p last_weight being the
 * past-the-end elements. Both ranges need to have the same size.
 * The function then calculates the weighted geometric mean of all positive finite elements
 * in the range. If no elements are finite, or if the range is empty, the returned value is `0.0`.
 * Non-finite numbers are ignored. If finite non-positive numbers (zero or negative) are found,
 * an exception is thrown. The weights have to be non-negative.
 *
 * For a set of values \f$ v \f$ and a set of weights \f$ w \f$,
 * the weighted geometric mean \f$ g \f$ is calculated following [1]:
 *
 * \f$ g = \exp \left( \frac{ \sum w \cdot \log v }{ \sum w } \right) \f$
 *
 * That is, if all weights are `1.0`, the formula yields the standard geometric mean.
 *
 * > [1] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,
 * > "A phylogenetic transform enhances analysis of compositional microbiota data,"
 * > Elife, vol. 6, p. e21887, Feb. 2017.
 * > https://elifesciences.org/articles/21887
 *
 * @see weighted_geometric_mean( std::vector<double> const& ) for a version for `std::vector`.
 * @see geometric_mean() for the unweighted version.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean.
 * @see weighted_arithmetic_mean() for a function that calculates the weighted arithmetic mean.
 */
template <class ForwardIterator>
double weighted_geometric_mean(
    ForwardIterator first_value,  ForwardIterator last_value,
    ForwardIterator first_weight, ForwardIterator last_weight
) {
    double num = 0.0;
    double den = 0.0;
    size_t cnt = 0;

    // Multiply elements.
    for_each_finite_pair( first_value, last_value, first_weight, last_weight, [&]( double value, double weight ){
        if( value <= 0.0 ) {
            throw std::invalid_argument(
                "Cannot calculate weighted geometric mean of non-positive values."
            );
        }
        if( weight < 0.0 ) {
            throw std::invalid_argument(
                "Cannot calculate weighted geometric mean with negative weights."
            );
        }

        num += weight * std::log( value );
        den += weight;
        ++cnt;
    });

    // If there are no valid elements, return an all-zero result.
    if( cnt == 0 ) {
        return 0.0;
    }
    if( den == 0.0 ) {
        throw std::invalid_argument(
            "Cannot calculate weighted geometric mean with all weights being 0."
        );
    }

    // Return the result.
    assert( cnt > 0 );
    assert( den > 0.0 );
    return std::exp( num / den );
}

/**
 * @brief Calculate the weighted geometric mean of a `std::vector` of `double` elements.
 *
 * @see weighted_geometric_mean( ForwardIterator first, ForwardIterator last ) for details.
 * @see geometric_mean() for the unweighted version.
 * @see arithmetic_mean() for a function that calculates the arithmetic mean.
 * @see weighted_arithmetic_mean() for a function that calculates the weighted arithmetic mean.
 */
inline double weighted_geometric_mean(
    std::vector<double> const& values,
    std::vector<double> const& weights
) {
    return weighted_geometric_mean( values.begin(), values.end(), weights.begin(), weights.end() );
}

// =================================================================================================
//     Median
// =================================================================================================

/**
 * @brief Calculate the median value of a sorted range of `double` values.
 *
 * The iterators are as usual: @p first points to the first element of the range,
 * @p last to the past-the-end element.
 *
 * The median of an odd sized range is its middle element; the median of an even sized range
 * is the arithmetic mean (average) of its two middle elements.
 */
template <class RandomAccessIterator>
double median( RandomAccessIterator first, RandomAccessIterator last )
{
    // Checks.
    if( ! std::is_sorted( first, last )) {
        throw std::runtime_error( "Range has to be sorted for median calculation." );
    }
    auto const size = static_cast<size_t>( std::distance( first, last ));
    if( size == 0 ) {
        return 0.0;
    }

    // Even or odd size? Median is calculated differently.
    if( size % 2 == 0 ) {

        // Get the two middle positions.
        size_t pl = size / 2 - 1;
        size_t pu = size / 2;
        assert( pl < size && pu < size );

        return ( *(first + pl) + *(first + pu) ) / 2.0;

    } else {

        // Int division, rounds down. This is what we want.
        size_t p = size / 2;
        assert( p < size );

        return *(first + p);
    }
}

/**
 * @brief Calculate the median value of a `vector` of `double`.
 *
 * The vector has to be sorted.
 */
inline double median( std::vector<double> const& vec )
{
    return median( vec.begin(), vec.end() );
}

// =================================================================================================
//     Quartiles
// =================================================================================================

/**
 * @brief Calculate the Quartiles of a sorted range of `double` values.
 *
 * The iterators are as usual: @p first points to the first element of the range,
 * @p last to the past-the-end element.
 */
template <class RandomAccessIterator>
Quartiles quartiles( RandomAccessIterator first, RandomAccessIterator last )
{
    // Prepare result.
    Quartiles result;

    // Checks.
    if( ! std::is_sorted( first, last )) {
        throw std::runtime_error( "Range has to be sorted for quartiles calculation." );
    }
    auto const size = static_cast<size_t>( std::distance( first, last ));
    if( size == 0 ) {
        return result;
    }

    // Set min, 50% and max.
    result.q0 = *first;
    result.q2 = median( first, last );
    result.q4 = *(first + size - 1);

    // Even or odd size? Quartiles are calculated differently.
    // This could be done shorter, but this way is more expressive.
    if( size % 2 == 0 ) {

        // Even: Split exaclty in halves.
        result.q1 = median( first, first + size / 2 );
        result.q3 = median( first + size / 2, first + size );

    } else {

        // Odd: Do not include the median value itself.
        result.q1 = median( first, first + size / 2 );
        result.q3 = median( first + size / 2 + 1, first + size );
    }

    return result;
}

/**
 * @brief Calculate the Quartiles of a `vector` of `double`.
 *
 * The vector has to be sorted.
 */
inline Quartiles quartiles( std::vector<double> const& vec )
{
    return quartiles( vec.begin(), vec.end() );
}

// =================================================================================================
//     Dispersion
// =================================================================================================

/**
 * @brief Calculate the index of dispersion.
 *
 * The coefficient of variation (CV), also known as the relative standard deviation (RSD),
 * is defined as the ratio of the standard deviation to the mean.
 * See mean_stddev() to calculate those values.
 * See https://en.wikipedia.org/wiki/Coefficient_of_variation for details.
 */
inline double coefficient_of_variation( MeanStddevPair const& ms )
{
    return ms.stddev / ms.mean;
}

/**
 * @copydoc coefficient_of_variation( MeanStddevPair const& ms )
 */
inline std::vector<double> coefficient_of_variation( std::vector<MeanStddevPair> const& ms )
{
    auto res = std::vector<double>( ms.size() );
    for( size_t i = 0; i < ms.size(); ++i ) {
        res[ i ] = coefficient_of_variation( ms[i] );
    }
    return res;
}

/**
 * @brief Calculate the index of dispersion.
 *
 * The index of dispersion, also known as the dispersion index, coefficient of dispersion,
 * relative variance, variance-to-mean ratio (VMR) or Fano factor, is defined as the ratio of the
 * variance to the mean. Variance is the square of the standard deviation.
 * See mean_stddev() to calculate those values.
 * See https://en.wikipedia.org/wiki/Index_of_dispersion for details.
 */
inline double index_of_dispersion( MeanStddevPair const& ms )
{
    return ms.stddev * ms.stddev / ms.mean;
}

/**
 * @copydoc index_of_dispersion( MeanStddevPair const& ms )
 */
inline std::vector<double> index_of_dispersion( std::vector<MeanStddevPair> const& ms )
{
    auto res = std::vector<double>( ms.size() );
    for( size_t i = 0; i < ms.size(); ++i ) {
        res[ i ] = index_of_dispersion( ms[i] );
    }
    return res;
}

/**
 * @brief Calculate the quartile_coefficient_of_dispersion.
 *
 * The quartile coefficient of dispersion is defined as `( Q3 - Q1 ) / ( Q3 + Q1 )`.
 * See quartiles() to caculate those values.
 * See https://en.wikipedia.org/wiki/Quartile_coefficient_of_dispersion for details.
 */
inline double quartile_coefficient_of_dispersion( Quartiles const& q )
{
    return ( q.q3 - q.q1 ) / ( q.q3 + q.q1 );
}

/**
 * @copydoc quartile_coefficient_of_dispersion( Quartiles const& ms )
 */
inline std::vector<double> quartile_coefficient_of_dispersion( std::vector<Quartiles> const& q )
{
    auto res = std::vector<double>( q.size() );
    for( size_t i = 0; i < q.size(); ++i ) {
        res[ i ] = quartile_coefficient_of_dispersion( q[i] );
    }
    return res;
}

// =================================================================================================
//     Correlation Coefficients
// =================================================================================================

/**
 * @brief Calculate the Pearson Correlation Coefficient between two ranges of `double`.
 *
 * Both ranges need to have the same length. Then, the function calculates the PCC
 * between the pairs of entries of both ranges. It skipes entries where any of the two values
 * is not finite.
 *
 * If each pair of entries in the ranges contains at leat one non-finite value, that is, if there
 * are no pairs of finite values, a `quiet_NaN` is returned. Furtheremore, if one of the ranges
 * has a standard deviation of `0.0`, e.g., because all its entries are `0.0` themselves,
 * a division by 0 occurs, leading to a `NaN` as well.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double pearson_correlation_coefficient(
    ForwardIteratorA first_a, ForwardIteratorA last_a,
    ForwardIteratorB first_b, ForwardIteratorB last_b
) {
    // Calculate means.
    double mean_a = 0.0;
    double mean_b = 0.0;
    size_t count = 0;
    for_each_finite_pair( first_a, last_a, first_b, last_b, [&]( double val_a, double val_b ){
        mean_a += val_a;
        mean_b += val_b;
        ++count;
    });
    if( count == 0 ) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    assert( count > 0 );
    mean_a /= static_cast<double>( count );
    mean_b /= static_cast<double>( count );

    // Calculate PCC parts.
    double numerator = 0.0;
    double stddev_a  = 0.0;
    double stddev_b  = 0.0;
    for_each_finite_pair( first_a, last_a, first_b, last_b, [&]( double val_a, double val_b ){
        double const d1 = val_a - mean_a;
        double const d2 = val_b - mean_b;
        numerator += d1 * d2;
        stddev_a  += d1 * d1;
        stddev_b  += d2 * d2;
    });

    // Calculate PCC, and assert that it is in the correct range
    // (or not a number, which can happen if the std dev is 0.0, e.g. in all-zero vectors).
    auto const pcc = numerator / ( std::sqrt( stddev_a ) * std::sqrt( stddev_b ) );
    assert(( -1.0 <= pcc && pcc <= 1.0 ) || ( ! std::isfinite( pcc ) ));
    return pcc;
}

/**
 * @brief Calculate the Pearson Correlation Coefficient between the entries of two vectors.
 *
 * @copydetails pearson_correlation_coefficient( ForwardIteratorA first_a, ForwardIteratorA last_a, ForwardIteratorB first_b, ForwardIteratorB last_b ).
 */
inline double pearson_correlation_coefficient(
    std::vector<double> const& vec_a,
    std::vector<double> const& vec_b
) {
    return pearson_correlation_coefficient(
        vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end()
    );
}

/**
 * @brief Calculate Spearman's Rank Correlation Coefficient between two ranges of `double`.
 *
 * Both ranges need to have the same length. Then, the function calculates Spearmans's Rho
 * between the pairs of entries of both vectors. Ranking is done via
 * @link ranking_fractional() fractional ranking@endlink.
 * Pairs of entries which contain non-finite values are skipped.
 */
template <class RandomAccessIteratorA, class RandomAccessIteratorB>
double spearmans_rank_correlation_coefficient(
    RandomAccessIteratorA first_a, RandomAccessIteratorA last_a,
    RandomAccessIteratorB first_b, RandomAccessIteratorB last_b
) {
    // Get cleaned results. We need to make these copies, as we need to calculate the fractional
    // ranking on them, which would change if we used our normal for_each_finite_pair here...
    auto const cleaned = finite_pairs( first_a, last_a, first_b, last_b );

    // Get the ranking of both vectors.
    auto ranks_a = ranking_fractional( cleaned.first );
    auto ranks_b = ranking_fractional( cleaned.second );
    assert( ranks_a.size() == ranks_b.size() );

    return pearson_correlation_coefficient( ranks_a, ranks_b );
}

/**
 * @brief Calculate Spearman's Rank Correlation Coefficient between the entries of two vectors.
 *
 * @copydetails spearmans_rank_correlation_coefficient( RandomAccessIteratorA first_a, RandomAccessIteratorA last_a, RandomAccessIteratorB first_b, RandomAccessIteratorB last_b )
 */
inline double spearmans_rank_correlation_coefficient(
    std::vector<double> const& vec_a,
    std::vector<double> const& vec_b
) {
    return spearmans_rank_correlation_coefficient(
        vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end()
    );
}

/**
 * @brief Apply Fisher z-transformation to a correlation coefficient.
 *
 * The coefficient can be calculated with pearson_correlation_coefficient() or
 * spearmans_rank_correlation_coefficient() and has to be in range `[ -1.0, 1.0 ]`.
 *
 * There is also a version of this function for a vector of coefficients.
 */
inline double fisher_transformation( double correlation_coefficient )
{
    auto const r = correlation_coefficient;
    if( r < -1.0 || r > 1.0 ) {
        throw std::invalid_argument(
            "Cannot apply fisher transformation to value " + std::to_string( r ) +
            " outside of [ -1.0, 1.0 ]."
        );
    }

    // LOG_DBG << "formula " << 0.5 * log( ( 1.0 + r ) / ( 1.0 - r ) );
    // LOG_DBG << "simple  " << std::atanh( r );
    return std::atanh( r );
}

/**
 * @brief Apply Fisher z-transformation to a vector of correlation coefficients.
 *
 * See fisher_transformation( double ) for details.
 */
inline std::vector<double> fisher_transformation( std::vector<double> const& correlation_coefficients )
{
    auto res = correlation_coefficients;
    for( auto& elem : res ) {
        elem = fisher_transformation( elem );
    }
    return res;
}

// =================================================================================================
//     Linear Regression
// =================================================================================================

/**
 * @brief Data structer to keep the two parameters of a linear function:
 * its @link LinearFunction::slope slope@endlink,
 * and its @link LinearFunction::intercept intercept@endlink.
 *
 * It also has a function to calcualte `y = slope * x + intercept`.
 */
struct LinearFunction
{
    double slope;
    double intercept;

    double y( double x )
    {
        return slope * x + intercept;
    }
};

/**
 * @brief Simple linear regression, for predicting the dependent variable `y` given the independent
 * variable `x`, using ordinary least squares regression.
 *
 * See https://en.wikipedia.org/wiki/Simple_linear_regression for an explanation.
 *
 * @see mean_squared_error() for calculating the resulting error.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
LinearFunction simple_linear_regression(
    ForwardIteratorA first_x, ForwardIteratorA last_x,
    ForwardIteratorB first_y, ForwardIteratorB last_y
) {
    // Calculate means of x and y := Mean(x), Mean(y) in parallel.
    double mean_x = 0.0;
    double mean_y = 0.0;
    size_t count = 0;
    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double val_x, double val_y ){
        mean_x += val_x;
        mean_y += val_y;
        ++count;
    });
    if( count == 0 ) {
        return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };
    }
    assert( count > 0 );
    mean_x /= static_cast<double>( count );
    mean_y /= static_cast<double>( count );
    assert( std::isfinite( mean_x ));
    assert( std::isfinite( mean_y ));

    // Calculate Cov(x,y) := covariance of x and y, and Var(x) := variance of x.
    double covariance = 0.0;
    double variance_x = 0.0;
    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double val_x, double val_y ){
        double const dx = val_x - mean_x;
        double const dy = val_y - mean_y;
        covariance += dx * dy;
        variance_x += dx * dx;
    });
    assert( std::isfinite( covariance ));
    assert( std::isfinite( variance_x ));

    // The linear parameters are slope := Cov(x,y) / Var(x), and intercept = Mean(y) - slope * Mean(x).
    LinearFunction result;
    result.slope = covariance / variance_x;
    result.intercept = mean_y - result.slope * mean_x;
    return result;
}

/**
 * @brief Calculate the mean squared error obtained from a linear fit of the input variables.
 *
 * The error per data point `(x,y)` is calculated as the squared differences between `y`
 * and the prediction given by the linear function @p lin_fct for `x`.
 * The function returns the mean of the errors for all data points.
 *
 * @see simple_linear_regression() for calculating such a fit.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double mean_squared_error(
    ForwardIteratorA first_x, ForwardIteratorA last_x,
    ForwardIteratorB first_y, ForwardIteratorB last_y,
    LinearFunction lin_fct
) {
    double error = 0.0;
    size_t count = 0;

    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double val_x, double val_y ){
        double const e = val_y - lin_fct.y( val_x );
        error += e * e;
        ++count;
    });

    if( count == 0 ) {
        assert( error == 0.0 );
        return error;
    }
    return error / static_cast<double>( count );
}

/**
 * @brief Calculate the fraction of unexplained variance resulting from a linear fit of the input
 * variables.
 *
 * See https://en.wikipedia.org/wiki/Fraction_of_variance_unexplained for some details.
 *
 * @see simple_linear_regression() for calculating such a fit.
 */
template <class ForwardIteratorA, class ForwardIteratorB>
double fraction_of_variance_unexplained(
    ForwardIteratorA first_x, ForwardIteratorA last_x,
    ForwardIteratorB first_y, ForwardIteratorB last_y,
    LinearFunction lin_fct
) {
    // Prepare mean of y and the count of valid value pairs.
    double y_mean = 0.0;
    size_t count = 0;

    // First get mean of y. We use the x iterator only to make sure that we skip invalid pairs.
    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double, double y_val ){
        y_mean += y_val;
        ++count;
    });
    y_mean /= static_cast<double>( count );

    // Edge case.
    if( count == 0 ) {
        return 0.0;
    }

    // Prepare sums of squares.
    double ss_err = 0.0;
    double ss_tot = 0.0;
    // double ss_reg = 0.0;

    // Iterate again, this time calculate the components needed.
    for_each_finite_pair( first_x, last_x, first_y, last_y, [&]( double x_val, double y_val ){
        double const y_hat = lin_fct.y( x_val );

        double const d_err = y_val - y_hat;
        double const d_tot = y_val - y_mean;
        // double const d_reg = y_hat - y_mean;

        ss_err += d_err * d_err;
        ss_tot += d_tot * d_tot;
        // ss_reg += d_reg * d_reg;
    });

    auto const fvu = ( ss_err / ss_tot );
    // auto const fvu = ( 1.0 - ss_reg / ss_tot );
    assert( 0.0 <= fvu && fvu <= 1.0 );
    return fvu;
}

} // namespace utils
} // namespace genesis

#endif // include guard
