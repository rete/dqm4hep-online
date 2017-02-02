  /// \file Data.h
/*
 *
 * Data.h header template automatically generated by a class generator
 * Creation date : jeu. f�vr. 2 2017
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef DATA_H
#define DATA_H

#include "dqm4hep/DQM4HEP.h"

namespace dqm4hep {

  namespace core {

    /**
     * Point class
     */
    struct Point
    {
      /**
       * Constructor
       */
      Point();

      /**
       * Constructor
       *
       * @param value the point value
       */
      Point(const float &value);

      /**
      * Constructor
      *
      * @param value the point value
      * @param error the error on the point value (up and down)
       */
      Point(const float &value, const float &error);

      /**
      * Constructor
      *
      * @param value the point value
      * @param errorUp the upper error on the point value
      * @param errorDown the down error on the point value
       */
      Point(const float &value, const float &errorUp, const float &errorDown);

      float              m_value;
      float              m_errorUp;
      float              m_errorDown;
    };

    typedef std::vector<Point> PointList;

    /**
     *
     */
    struct DataSet1D
    {
      PointList               m_points;
    };

    /**
     *
     */
    struct DataSet2D
    {
      PointList               m_pointsX;
      PointList               m_pointsY;
    };

    /**
     *
     */
    struct DataSet3D
    {
      PointList               m_pointsX;
      PointList               m_pointsY;
      PointList               m_pointsZ;
    };

  }

}

#endif  //  DATA_H