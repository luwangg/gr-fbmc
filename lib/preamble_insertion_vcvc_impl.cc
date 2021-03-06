/* -*- c++ -*- */
/* 
 * Copyright 2014 Communications Engineering Lab (CEL), Karlsruhe Institute of Technology (KIT).
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "preamble_insertion_vcvc_impl.h"

namespace gr {
  namespace fbmc {

    preamble_insertion_vcvc::sptr
    preamble_insertion_vcvc::make(int L, int frame_len, std::vector<gr_complex> preamble_sym)
    {
      return gnuradio::get_initial_sptr
        (new preamble_insertion_vcvc_impl(L, frame_len, preamble_sym));
    }

    /*
     * The private constructor
     */
    preamble_insertion_vcvc_impl::preamble_insertion_vcvc_impl(int L, int frame_len, std::vector<gr_complex> preamble_sym)
      : gr::sync_block("preamble_insertion_vcvc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)*L),
              gr::io_signature::make(1, 1, sizeof(gr_complex)*L)),
              d_L(L),
              d_frame_len(frame_len), // frame_len as given relates to the pre-OQAM symbol rate
              d_ctr(0),
              d_preamble_sym(preamble_sym)
    {
      if((d_preamble_sym.size() % d_L) != 0){
        throw std::runtime_error(
            "Preamble size must be an integer multiple of the number of subcarriers");
      }
    }

    /*
     * Our virtual destructor.
     */
    preamble_insertion_vcvc_impl::~preamble_insertion_vcvc_impl()
    {
    }

    inline void
    preamble_insertion_vcvc_impl::process_one_symbol(gr_complex* outbuf,
                                                     const gr_complex* inbuf)
    {
      if(d_ctr < d_preamble_sym.size() / d_L){ // insert preamble symbol
        memcpy(outbuf, &d_preamble_sym[0] + d_ctr * d_L, sizeof(gr_complex) * d_L);
      }
      else{
        // just copy the input to the output
        memcpy(outbuf, inbuf, sizeof(gr_complex) * d_L);
      }

      d_ctr = (d_ctr + 1) % d_frame_len;
    }

    int
    preamble_insertion_vcvc_impl::work(int noutput_items,
                                       gr_vector_const_void_star &input_items,
                                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      for(int i = 0; i < noutput_items; i++){
        process_one_symbol(out, in);
        in += d_L;
        out += d_L;
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace fbmc */
} /* namespace gr */

