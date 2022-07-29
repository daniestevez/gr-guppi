/* -*- c++ -*- */
/*
 * Copyright 2022 Daniel Estevez <daniel@destevez.net>.
 *
 * This file is part of gr-guppi
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "guppi_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <algorithm>
#include <cstring>

namespace gr {
namespace guppi {

guppi_sink::sptr guppi_sink::make(const char* filename,
                                  size_t obsnchan,
                                  size_t ndim,
                                  size_t npol,
                                  double obsfreq,
                                  double obsbw)
{
    return gnuradio::make_block_sptr<guppi_sink_impl>(
        filename, obsnchan, ndim, npol, obsfreq, obsbw);
}


guppi_sink_impl::guppi_sink_impl(const char* filename,
                                 size_t obsnchan,
                                 size_t ndim,
                                 size_t npol,
                                 double obsfreq,
                                 double obsbw)
    : gr::sync_block(
          "guppi_sink",
          gr::io_signature::make(npol, npol, 2 * obsnchan * sizeof(std::int8_t)),
          gr::io_signature::make(0, 0, 0)),
      d_obsnchan(obsnchan),
      d_ndim(ndim),
      d_npol(npol),
      d_obsfreq(obsfreq * 1e-6),
      d_obsbw(obsbw * 1e-6),
      d_tbin(obsnchan / obsbw),
      d_chan_bw(1e-6 * obsbw / obsnchan),
      d_blocsize(2 * ndim * obsnchan * npol * d_nbits / 8),
      d_pktidx(0),
      d_nsamples_stored(0)
{
    if ((d_npol != 1) && (d_npol != 2)) {
        throw std::runtime_error("[guppi_sink] npol should be either 1 or 2");
    }

    d_buffer.resize(d_blocsize);
    if (!(d_fp = fopen(filename, "wb"))) {
        throw std::runtime_error("[guppi_sink] failed to open output file");
    }
}

guppi_sink_impl::~guppi_sink_impl() { fclose(d_fp); }

int guppi_sink_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    const size_t bytes_per_iq_sample = 2 * d_nbits / 8;

    for (int j = 0; j < noutput_items; ++j) {
        for (size_t pol = 0; pol < d_npol; ++pol) {
            auto in = static_cast<const std::uint8_t*>(input_items[pol]);
            for (size_t chan = 0; chan < d_obsnchan; ++chan) {
                size_t output_offset =
                    ((chan * d_ndim + d_nsamples_stored) * d_npol + pol) *
                    bytes_per_iq_sample;
                size_t input_offset = (j * d_obsnchan + chan) * bytes_per_iq_sample;
                std::memcpy(
                    &d_buffer[output_offset], &in[input_offset], bytes_per_iq_sample);
            }
        }
        if (++d_nsamples_stored == d_ndim) {
            // A block is complete
            write_guppi_block();
            ++d_pktidx;
            d_nsamples_stored = 0;
        }
    }

    return noutput_items;
}

void guppi_sink_impl::write_guppi_block()
{
    // Write header
    int len;
    prepare_header_line();
    len = snprintf(d_header_line, 80, "OBSFREQ = %e", d_obsfreq);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "OBSBW = %e", d_obsbw);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "OBSNCHAN = %zu", d_obsnchan);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "NPOL = %zu", d_npol);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "NBITS = %u", d_nbits);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "TBIN = %e", d_tbin);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "CHAN_BW = %e", d_chan_bw);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "OVERLAP = %u", d_overlap);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "BLOCSIZE = %zu", d_blocsize);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "PKTIDX = %zu", d_pktidx);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "END");
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    // Write data
    if (fwrite(d_buffer.data(), d_blocsize, 1, d_fp) != 1) {
        throw std::runtime_error("[guppi_sink] failed to write samples to output file");
    }
}

} /* namespace guppi */
} /* namespace gr */
