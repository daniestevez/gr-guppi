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
                                  double obsbw,
                                  const char* telescope,
                                  const char* observer,
                                  const char* src_name,
                                  const char* proj_id,
                                  double ra,
                                  double dec,
                                  double az,
                                  double za,
                                  double stt_mjd)
{
    return gnuradio::make_block_sptr<guppi_sink_impl>(filename,
                                                      obsnchan,
                                                      ndim,
                                                      npol,
                                                      obsfreq,
                                                      obsbw,
                                                      telescope,
                                                      observer,
                                                      src_name,
                                                      proj_id,
                                                      ra,
                                                      dec,
                                                      az,
                                                      za,
                                                      stt_mjd);
}


guppi_sink_impl::guppi_sink_impl(const char* filename,
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
                                 double stt_mjd)
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
      d_nsamples_stored(0),
      d_telescope(telescope),
      d_observer(observer),
      d_src_name(src_name),
      d_proj_id(proj_id),
      d_ra(ra),
      d_dec(dec),
      d_az(az),
      d_za(za),
      d_stt_mjd(stt_mjd)
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
    len = snprintf(d_header_line, 80, "BACKEND = 'GUPPI'");
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "TELESCOP = '%s'", d_telescope.c_str());
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "OBSERVER = '%s'", d_observer.c_str());
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "SRC_NAME = '%s'", d_src_name.c_str());
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "PROJID = '%s'", d_proj_id.c_str());
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    double ra_hours = d_ra / 15.0;
    int ra_hours_i = static_cast<int>(ra_hours);
    double ra_minutes = (ra_hours - ra_hours_i) * 60.0;
    int ra_minutes_i = static_cast<int>(ra_minutes);
    double ra_seconds = (ra_minutes - ra_minutes_i) * 60.0;
    len = snprintf(d_header_line,
                   80,
                   "RA_STR = '%02d:%02d:%02.4f'",
                   ra_hours_i,
                   ra_minutes_i,
                   ra_seconds);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "RA = %.4f", d_ra);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    int dec_deg_i = static_cast<int>(d_dec);
    double dec_minutes = (d_dec - dec_deg_i) * 60.0;
    int dec_minutes_i = static_cast<int>(dec_minutes);
    double dec_seconds = (dec_minutes - dec_minutes_i) * 60.0;
    len = snprintf(d_header_line,
                   80,
                   "DEC_STR = '%+02d:%02d:%02.4f'",
                   dec_deg_i,
                   dec_minutes_i,
                   dec_seconds);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "DEC = %.4f", d_dec);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "AZ = %.4f", d_az);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "ZA = %.4f", d_za);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    int imjd = static_cast<int>(d_stt_mjd);
    int smjd = static_cast<int>((d_stt_mjd - imjd) * 86400.0);

    prepare_header_line();
    len = snprintf(d_header_line, 80, "STT_SMJD = %d", smjd);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "STT_IMJD = %d", imjd);
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

    prepare_header_line();
    len = snprintf(d_header_line, 80, "STTVALID = 1");
    d_header_line[std::min(len, 79)] = ' ';
    write_header_line();

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
