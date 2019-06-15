/*//NOTE  read 2019.3.31 18:47
 * CitcomCU is a Finite Element Code that solves for thermochemical
 * convection within a three dimensional domain appropriate for convection
 * within the Earth's mantle. Cartesian and regional-spherical geometries
 * are implemented. See the file README contained with this distribution
 * for further details.
 * 
 * Copyright (C) 1994-2005 California Institute of Technology
 * Copyright (C) 2000-2005 The University of Colorado
 *
 * Authors: Louis Moresi, Shijie Zhong, and Michael Gurnis
 *
 * For questions or comments regarding this software, you may contact
 *
 *     Luis Armendariz <luis@geodynamics.org>
 *     http://geodynamics.org
 *     Computational Infrastructure for Geodynamics (CIG)
 *     California Institute of Technology
 *     2750 East Washington Blvd, Suite 210
 *     Pasadena, CA 91007
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 2 of the License, or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*  Functions which construct the shape function values at all of the gauss
    points in the element (including the reduced quadrature points). The element in question is
    biquadratic in the velocities and therefore bilinear in the pressures. 
    
    To change elements it is necessary to change this file: Shape_functions.c,
    and the element-data header file : element_definitions.h  but it should not be
    necessary to change the main calculation/setup/solving machinery.		 */

#include <math.h>
#include "element_definitions.h"
#include "global_defs.h"

/*  =======================================================
    Function creating shape_fn data in form of a structure
    =======================================================*/

void construct_shape_functions(struct All_variables *E)
{
	//int i, j, k, d, dd, el;
	int i, j, k, d, dd;
	//int remapj, remapk;

	/* first zero ALL entries, even those not used in 2d. */

	for(i = 0; i < GNVI; i++)
	{
		E->N.vpt[i] = 0.0;
		E->Nx.vpt[i] = 0.0;
		E->Nx.vpt[GNVI + i] = 0.0;
		E->Nx.vpt[2 * GNVI + i] = 0.0;
	}

	for(i = 0; i < GNPI; i++)
	{
		E->N.ppt[i] = 0.0;
		E->Nx.ppt[i] = 0.0;
		E->Nx.ppt[GNPI + i] = 0.0;
		E->Nx.ppt[2 * GNPI + i] = 0.0;
	}

	for(i = 0; i < GN1VI; i++)
	{
		E->M.vpt[i] = 0.0;                       //TODO M是什么? M.vpt[16],M只有两个导数
		E->Mx.vpt[i] = 0.0;
		E->Mx.vpt[GN1VI + i] = 0.0;
	}

	for(i = 0; i < GN1PI; i++)
	{
		E->M.ppt[i] = 0.0;
		E->Mx.ppt[i] = 0.0;
		E->Mx.ppt[GN1PI + i] = 0.0;
	}

	for(i = 0; i < GN1VI; i++)					//TODO same as before
	{
		E->L.vpt[i] = 0.0;
		E->Lx.vpt[i] = 0.0;
		E->Lx.vpt[GN1VI + i] = 0.0;
	}


	for(i = 1; i <= enodes[E->mesh.nsd]; i++)
	{
		/*  for each node  */

		for(j = 1; j <= vpoints[E->mesh.nsd]; j++)
		{

			/* for each integration point  */
			E->N.vpt[GNVINDEX(i, j)] = 1.0;
			for(d = 1; d <= E->mesh.nsd; d++)
			{
				E->N.vpt[GNVINDEX(i, j)] *= lpoly(bb[d - 1][i], g_point[j].x[d - 1]);
			}
			for(dd = 1; dd <= E->mesh.nsd; dd++)
			{
				E->Nx.vpt[GNVXINDEX(dd - 1, i, j)] = lpolydash(bb[dd - 1][i], g_point[j].x[dd - 1]);
				for(d = 1; d <= E->mesh.nsd; d++)
					if(d != dd)
						E->Nx.vpt[GNVXINDEX(dd - 1, i, j)] *= lpoly(bb[d - 1][i], g_point[j].x[d - 1]);
			}
		}


		for(j = 1; j <= ppoints[E->mesh.nsd]; j++)   // j only for 1
		{
			/* for each p-integration point  */
			E->N.ppt[GNPINDEX(i, j)] = 1.0;
			for(d = 1; d <= E->mesh.nsd; d++)
			{
				E->N.ppt[GNPINDEX(i, j)] *= lpoly(bb[d - 1][i], p_point[j].x[d - 1]);  //TODO p为何也是拉格朗日函数形式 ？
			}
			for(dd = 1; dd <= E->mesh.nsd; dd++)
			{
				E->Nx.ppt[GNPXINDEX(dd - 1, i, j)] = lpolydash(bb[dd - 1][i], p_point[j].x[dd - 1]);
				for(d = 1; d <= E->mesh.nsd; d++)
					if(d != dd)
					{
						E->Nx.ppt[GNPXINDEX(dd - 1, i, j)] *= lpoly(bb[d - 1][i], p_point[j].x[d - 1]);
					}
			}
		}

	}



	for(j = 1; j <= onedvpoints[E->mesh.nsd]; j++)
		for(k = 1; k <= onedvpoints[E->mesh.nsd]; k++)
		{
			E->M.vpt[GMVINDEX(j, k)] = 1.0;
			E->L.vpt[GMVINDEX(j, k)] = 1.0;
			for(d = 1; d <= E->mesh.nsd - 1; d++)
			{
				E->M.vpt[GMVINDEX(j, k)] *= lpoly(bb[d - 1][j], s_point[k].x[d - 1]);
				E->L.vpt[GMVINDEX(j, k)] *= lpoly(bb[d - 1][j], l_1d[k].x[d - 1]);
			}

			for(dd = 1; dd <= E->mesh.nsd - 1; dd++)
			{
				E->Mx.vpt[GMVXINDEX(dd - 1, j, k)] = lpolydash(bb[dd - 1][j], s_point[k].x[d - 1]);
				E->Lx.vpt[GMVXINDEX(dd - 1, j, k)] = lpolydash(bb[dd - 1][j], l_1d[k].x[d - 1]);
				for(d = 1; d <= E->mesh.nsd - 1; d++)
					if(d != dd)
					{
						E->Mx.vpt[GMVXINDEX(dd - 1, j, k)] *= lpoly(bb[d - 1][j], s_point[k].x[d - 1]);
						E->Lx.vpt[GMVXINDEX(dd - 1, j, k)] *= lpoly(bb[d - 1][j], l_1d[k].x[d - 1]);
					}
			}
		}

	return;
}


/* lpoly
 *	p - selects lagrange polynomial, 1d: node p
 *	y - coordinate in given direction to evaluate poly
 */
double lpoly(int p, double y)
{
	double value;

	switch (p)
	{
	case 1:
		value = 0.5 * (1 - y);
		break;
	case 2:
		value = 0.5 * (1 + y);
		break;
	default:
		value = 0.0;
	}

	return value;
}

double lpolydash(int p, double y)
{
	double value;
	switch (p)
	{
	case 1:
		value = -0.5;
		break;
	case 2:
		value = 0.5;
		break;
	default:
		value = 0.0;
	}

	return value;
}
