/* -*- c++ -*- */
/*
 * Copyright 2022 Daniel Estevez <daniel@destevez.net>.
 *
 * This file is part of gr-guppi
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GUPPI_GUPPI_SINK_H
#define INCLUDED_GUPPI_GUPPI_SINK_H

#include <gnuradio/guppi/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace guppi {

/*!
 * \brief GUPPI File Sink
 * \ingroup guppi
 *
 * \details
 *
 * The GUPPI File Sink block writes single or dual-polarization channelized
 * 8-bit samples into a GUPPI raw file.
 */
class GUPPI_API guppi_sink : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<guppi_sink> sptr;

    /*!
     * \brief Build the GUPPI File Sink block.
     *
     * \param filename Path of the output file
     * \param obsnchan Number of polyphase filterbank channels
     * \param ndim Number of samples (in each channel) to include per GUPPI file
     *  block
     * \param npol Number of polarizations (either 1 or 2)
     * \param obsfreq Frequency of the left edge of the central channel in Hz
     * \param obsbw Total bandwidth in Hz
     * \param telescope Telescope name
     * \param observer Observer on duty
     * \param src_name Source name
     * \param proj_id Project ID
     * \param ra Right ascension (in degrees)
     * \param dec Declination (in degrees)
     * \param az Azimuth (in degrees)
     * \param za Zenith angle (in degrees)
     * \param stt_mjd Start time MJD
     */
    static sptr make(const char* filename,
                     size_t obsnchan,
                     size_t ndim,
                     size_t npol,
                     double obsfreq,
                     double obsbw,
                     const char* telescope,
                     const char* observer,
                     const char* src_name,
                     const char* proj_id,
                     double ra,
                     double dec,
                     double az,
                     double za,
                     double stt_mjd);
};

} // namespace guppi
} // namespace gr

#endif /* INCLUDED_GUPPI_GUPPI_SINK_H */
