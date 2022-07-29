/* -*- c++ -*- */
/*
 * Copyright 2022 Daniel Estevez <daniel@destevez.net>.
 *
 * This file is part of gr-guppi
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GUPPI_GUPPI_SINK_IMPL_H
#define INCLUDED_GUPPI_GUPPI_SINK_IMPL_H

#include <gnuradio/guppi/guppi_sink.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace gr {
namespace guppi {

class guppi_sink_impl : public guppi_sink
{
private:
    // Constructor parameters
    const size_t d_obsnchan;
    const size_t d_ndim;
    const size_t d_npol;
    const double d_obsfreq;
    const double d_obsbw;
    // Computed from constructor parameters
    static const unsigned d_nbits = 8;
    const double d_tbin;
    const double d_chan_bw;
    static const unsigned d_overlap = 0;
    const size_t d_blocsize;
    // Updated while running
    size_t d_pktidx;
    // Buffer to store data
    std::vector<std::uint8_t> d_buffer;
    size_t d_nsamples_stored;
    // File
    FILE* d_fp;
    char d_header_line[80];

    void write_guppi_block();

    inline void prepare_header_line()
    {
        memset(d_header_line, ' ', sizeof(d_header_line));
    }

    inline void write_header_line() const
    {
        if (fwrite(d_header_line, sizeof(d_header_line), 1, d_fp) != 1) {
            throw std::runtime_error(
                "[guppi_sink] failed to write header to output file");
        }
    }

public:
    guppi_sink_impl(const char* filename,
                    size_t obsnchan,
                    size_t ndim,
                    size_t d_npol,
                    double obsfreq,
                    double obsbw);
    ~guppi_sink_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace guppi
} // namespace gr

#endif /* INCLUDED_GUPPI_GUPPI_SINK_IMPL_H */
