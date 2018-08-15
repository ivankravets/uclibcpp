/*	Copyright (C) 2018 Anton Mikhaylov aka anion155
	This file is part of the uClibc++ Library.
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#pragma once

namespace std {
	template <class T> class auto_ptr;

	template <class T> class unique_ptr : public auto_ptr<T> {
  public:
    using auto_ptr<T>::auto_ptr;

    void swap(unique_ptr &o) {
      auto o_obj = o.release();
      auto t_obj = this->release();
      o.reset(t_obj);
      this->reset(o_obj);
    }
  };

	template <class T, class ...Args> inline unique_ptr<T> make_unique(Args &&...args) {
    return unique_ptr<T>(new T(forward<Args>(args)...));
  }
}
