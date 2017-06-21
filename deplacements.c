
/*
Tous les temps sont exprimes en ms.
Toutes les distances sont exprimees en mm.
Convention moteurs :
 - motorA = moteur droit
 - motorB = moteur gauche
*/

/*
A faire pour la suite : ne jamais modifier les valeurs de nMotorEncoder[], sauf a l'initialisation.

Le sonar avant doit etre nomme "SonarAvant", sinon le programme ne foncitonnera pas.
*/

// A ajouter au debut du programme principal c.

// Variables globales privees.
float distance_faite;
float distance_a_faire;
float rotation_faite;
float rotation_a_faire;
char mvt_fait = 0;
char obstacle_rencontre_private = 0;


// Puissance maximale du moteur pour lequel il ne bouge pas. Si on demande une puissance > offset_motor, le moteur peut deplacer le robot.
// Seulement utile pour les briques matrix et tetrix.
char offset_motor=0;

float rayon = 40; // en mm
float entraxe = 220.0;

// 1 pour une brique Matrix, 2 pour une brique Tetrix, 0 pour une LEGO.
char matrix_or_tetrix=0;

// Si 0, on ne tient pas compte des obstacles qui sont devant. Si 1, on en tient compte.
char obstacle = 0;

// Distance a parcourir quand un obstacle est vu par le robot. Evite les discontinuite de vitesse.
float temp_dist_avant = 60;
float temp_dist_arriere = 60;

// Distance de detection 
int dist_detect_avant = 17;
int dist_detect_arriere = 15;

// Saturation sur le derivee de la puissance envoyee aux moteurs.
float saturation_dpuiss_avant = 3.0;
float saturation_dpuiss_arriere = 3.0;
float saturation_dpuiss_rotation = 3.0;

// Variable qui divise la puissance des moteurs pour les rotations.
float divise_rotation = 27.0;

// Puissance minimale sur les moteurs LEGO
int puiss_min_lego = 5;

// Permet de changer le sens de rotation des moteurs.
// A mettre a 1 ou a -1.
int reverse = 1;


void avance_nulle_obstacle(float distance, int encA, int encB)
{
	// Pas de check, cette fonction est la pour eviter la discontinuite de vitesse dans les moteurs.
	int const nbr_tranches=1;
	int tranches[]={600};
	float coeff[] = {4.0, 6.0};
	float denom = 0.75;
	float d = distance;
	if(matrix_or_tetrix == 1)
	{
		denom = 0.0;
		if(d > 70)
			d-=10;
		for(char i = 0; i < nbr_tranches; i++)
		{
			if(distance <= tranches[i])
			{
				denom = coeff[i];
				i = nbr_tranches;
			}
		}
		if(denom == 0.0)
			denom = coeff[nbr_tranches];
	}
	if(matrix_or_tetrix == 0)
		denom = 1.0/100.0;
	
	float dist = 0;
	float puiss = 0;
	float old_puiss = 0.0;
	float erreur = 0;
	float old_erreur = 0;
	
	int encA_old = encA;
	int encB_old = encB;
	
	
	while((abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0 < d)
	{
				dist = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
				
				// Ajoute pour la version avec les tasks.
				distance_faite = dist;
				erreur = d - dist;
				old_puiss = puiss;
				
				// Calcul de la puissance sur les moteurs.
				if(matrix_or_tetrix == 0)
				{
					puiss =(erreur*0.294-old_erreur*0.290)/denom+offset_motor;
					if(puiss <= puiss_min_lego)
						puiss=puiss_min_lego;
				}
				else if(matrix_or_tetrix == 1)
					puiss =(erreur*1.13-old_erreur*1.12 + (puiss-offset_motor)*1.8)/denom+offset_motor;
				else
					puiss =(erreur*0.43-old_erreur*0.42 + (puiss-offset_motor)*0.37)/denom+offset_motor;
				
				// Saturation de la derivee de la puissance.
				if(saturation_dpuiss_avant != 0)
				{
					if(puiss > old_puiss+saturation_dpuiss_avant)
						puiss = old_puiss+saturation_dpuiss_avant;
					else if(puiss < old_puiss-saturation_dpuiss_avant)
						puiss = old_puiss-saturation_dpuiss_avant;
				}
				
				//puiss =(erreur*1.13-old_erreur*1.12 + (puiss-12)*1.8)/3.0+12;
				old_erreur = erreur;
				motor[motorA] = reverse * (int) puiss;
				motor[motorB] = reverse * (int) puiss;
				encA_old = nMotorEncoder[motorA];
				encB_old = nMotorEncoder[motorB];
				wait1Msec(10);	
	}
	motor[motorA] = 0;
	motor[motorB] = 0;
	mvt_fait = 1;
	for(char i = 0; i < 15; i++)
	{
		wait1Msec(10);
		distance_faite = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
	}
	return;
}

void recule_nulle_obstacle(float distance, int encA, int encB)
{
	// Pas de check, cette fonction est la pour eviter la discontinuite de vitesse dans les moteurs.
	int const nbr_tranches=1;
	int tranches[]={600};
	float coeff[] = {4.5, 9.0};
	float denom = 0.75;
	float d = distance;
	
	
	if(matrix_or_tetrix == 1)
	{
		denom = 0.0;
		if(d > 70)
			d-=7;
		for(char i = 0; i < nbr_tranches; i++)
		{
			if(distance <= tranches[i])
			{
				denom = coeff[i];
				i = nbr_tranches;
			}
		}
		if(denom == 0.0)
			denom = coeff[nbr_tranches];

			}
	if(matrix_or_tetrix == 0)
		denom = 1.2/150.0;

	
	float dist = 0;
	float puiss = 0;
	float old_puiss = 0.0;
	float erreur = 0;
	float old_erreur = 0;
	
	int encA_old = encA;
	int encB_old = encB;
	
	
	while((abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0 < d)
	{
				dist = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
				
				// Ajoute pour la version avec les tasks.
				distance_faite = dist;
				erreur = d - dist;
				old_puiss = puiss;
				
				// Calcul de la puissance sur les moteurs.
				if(matrix_or_tetrix == 0)
				{
					puiss =(erreur*0.294-old_erreur*0.290)/denom+offset_motor;
					if(puiss <= puiss_min_lego)
						puiss=puiss_min_lego;
				}
				else if(matrix_or_tetrix == 1)
					puiss =(erreur*1.13-old_erreur*1.12 + (puiss-offset_motor)*1.8)/denom+offset_motor;
				else
					puiss =(erreur*0.43-old_erreur*0.42 + (puiss-offset_motor)*0.37)/denom+offset_motor;
				
				// Saturation de la derivee de la puissance.
				if(saturation_dpuiss_arriere != 0)
				{
					if(puiss > old_puiss+saturation_dpuiss_arriere)
						puiss = old_puiss+saturation_dpuiss_arriere;
					else if(puiss < old_puiss-saturation_dpuiss_arriere)
						puiss = old_puiss-saturation_dpuiss_arriere;
				}
				
				//puiss =(erreur*1.13-old_erreur*1.12 + (puiss-12)*1.8)/3.0+12;
				old_erreur = erreur;
				motor[motorA] = reverse * (int) -puiss;
				motor[motorB] = reverse * (int) -puiss;
				encA_old = nMotorEncoder[motorA];
				encB_old = nMotorEncoder[motorB];
				wait1Msec(10);	
	}
	motor[motorA] = 0;
	motor[motorB] = 0;
	mvt_fait = 1;
	for(char i = 0; i < 15; i++)
	{
		wait1Msec(10);
		distance_faite = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
	}
	return;
}

void avance_nulle_private(float distance)
{
	if(distance <= 0)
		return;

		// Comportement du deplacement obtenu par etude des moteurs, par automatique lineaire et par essais experimentaux.
		int const nbr_tranches=1;
		int tranches[]={600};
		float coeff[] = {4.0, 6.0};
		float denom = 0.75;
		float d = distance;

		if(matrix_or_tetrix == 1)
		{
			denom = 0.0;
			if(d > 70)
				d-=10;
			for(char i = 0; i < nbr_tranches; i++)
			{
				if(distance <= tranches[i])
				{
					denom = coeff[i];
					i = nbr_tranches;
				}
			}

			if(denom == 0.0)
				denom = coeff[nbr_tranches];
		}
		if(matrix_or_tetrix == 0)
			denom = 1.0/100.0;

		int encA=nMotorEncoder[motorA];
		int encB=nMotorEncoder[motorB];

		int encA_old = encA;
		int encB_old = encB;
		char compte_arret = 0;

		// Pour detecter si le robot arrive ?? faire tourner ses roues ou non.
		//int encA_old = encA;
		//int encB_old = encB;

		float dist = 0;
		float puiss = 0;
		float old_puiss = 0.0;
		float erreur = 0;
		float old_erreur = 0;

		if(obstacle == 1 && SensorValue(SonarAvant) < dist_detect_avant)
		{
			obstacle_rencontre_private = 1;
			mvt_fait = 2;
			motor[motorA] = 0;
			motor[motorB] = 0;
			return;
		}
			

		while((abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0 < d)
		{
			if(obstacle == 1 && SensorValue(SonarAvant) < dist_detect_avant)
			{
				obstacle_rencontre_private = 1;
				
				// Dans le cas ou il ne reste presque plus rien a parcourir.
				float temp_dist = temp_dist_avant;
				if(distance - (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0 <= temp_dist)
				{
					temp_dist = distance - (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
					avance_nulle_obstacle(temp_dist, encA, encB);
					//nxtDisplayTextLine(6, "obstacle_nulle");
					mvt_fait = 1;
				}
				else
				{
					//avance_nulle_obstacle((abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0 + temp_dist, encA, encB);
					avance_nulle_obstacle(temp_dist, encA, encB);
					mvt_fait = 2;
					//nxtDisplayTextLine(5, "obligation d'arret");
				}
				// On est normalement a l'arret.
				motor[motorA] = 0;
				motor[motorB] = 0;
				wait1Msec(150);
				distance_faite = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
				return;
			}
			else
			{
				// Voir si on garde la detection !!! Pas sur avec tous les elements ajoutes.
				// Detection pour savoir si on a cogne contre un mur, auquel cas on est a l'arret, les roues immobiles.
				if(abs(nMotorEncoder[motorA] - encA_old) < 3 || abs(nMotorEncoder[motorB] - encB_old) < 3)
				{
					compte_arret++;
					// On considere que l'on est a l'arret.
					if(compte_arret >= 100)
					{
						//nxtDisplayTextLine(5, "Compte_arret !");
						motor[motorA] = 0;
						motor[motorB] = 0;
						mvt_fait = 2;
						return;
					}
				}
				else
					compte_arret = 0;

				dist = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
				
				// Ajoute pour la version avec les tasks.
				distance_faite = dist;
				erreur = d - dist;
				old_puiss = puiss;
				
				//puiss =(erreur*5.0-old_erreur*3.5 + (puiss-12)*1.0)/50.0+12;
				//puiss =(erreur*1.13-old_erreur*1.12 + (puiss-12)*1.8)/6.0+12;
				if(matrix_or_tetrix == 0)
				{
					puiss =(erreur*0.294-old_erreur*0.290)/denom+offset_motor;
					if(puiss <= puiss_min_lego)
						puiss=puiss_min_lego;
				}
				else if(matrix_or_tetrix == 1)
					puiss =(erreur*1.13-old_erreur*1.12 + (puiss-offset_motor)*1.8)/denom+offset_motor;
				else
					puiss =(erreur*0.43-old_erreur*0.42 + (puiss-offset_motor)*0.37)/denom+offset_motor;
				
				// Saturation de la derivee de la puissance.
				if(saturation_dpuiss_avant != 0)
				{
					if(puiss > old_puiss+saturation_dpuiss_avant)
						puiss = old_puiss+saturation_dpuiss_avant;
					else if(puiss < old_puiss-saturation_dpuiss_avant)
						puiss = old_puiss-saturation_dpuiss_avant;
				}
				
				//puiss =(erreur*1.13-old_erreur*1.12 + (puiss-12)*1.8)/3.0+12;
				//nxtDisplayTextLine(1, "puiss = %f", puiss);
				old_erreur = erreur;
				motor[motorA] = reverse * (int) puiss;
				motor[motorB] = reverse * (int) puiss;
				encA_old = nMotorEncoder[motorA];
				encB_old = nMotorEncoder[motorB];
				wait1Msec(10);
			}
		}
		motor[motorB]=0;
		motor[motorA]=0;
		
		mvt_fait = 1;
		for(char i = 0; i < 15; i++)
		{
			wait1Msec(10);
			distance_faite = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
		}
}

// Gestion des obstacles pour les robots Matrix et Tetrix.

void recule_nulle_private(float distance)
{
	if(distance <= 0)
		return;


		// Comportement du deplacement obtenu par etude des moteurs, par automatique lineaire et par essais experimentaux.
		int const nbr_tranches=1;
		int tranches[]={600};
		float coeff[] = {4.5, 9.0};
		float denom = 0.75;
		float d = distance;

		if(matrix_or_tetrix == 1)
		{
			denom = 0.0;
			if(d > 70)
				d-=7;

			for(char i = 0; i < nbr_tranches; i++)
			{
				if(distance <= tranches[i])
				{
					denom = coeff[i];
					i = nbr_tranches;
				}
			}
			if(denom == 0.0)
				denom = coeff[nbr_tranches];
		}
		if(matrix_or_tetrix == 0)
			denom = 1.2/150.0;

		int encA=nMotorEncoder[motorA];
		int encB=nMotorEncoder[motorB];

		int encA_old = encA;
		int encB_old = encB;
		char compte_arret = 0;

		float dist = 0;
		float puiss = 0;
		float old_puiss = 0.0;
		float erreur = 0;
		float old_erreur = 0;

		if(obstacle == 1 && SensorValue(SonarArriere) < dist_detect_arriere)
		{
			obstacle_rencontre_private = 1;
			mvt_fait = 2;
			motor[motorA] = 0;
			motor[motorB] = 0;
			return;
		}
		
		while(abs(nMotorEncoder[motorB]-encB)*PI*rayon/360.0 < d)
		{
			if(obstacle == 1 && SensorValue(SonarArriere) < dist_detect_arriere)
			{
				obstacle_rencontre_private = 1;
				
				// Dans le cas ou il ne reste presque plus rien a parcourir.
				float temp_dist = temp_dist_arriere;
				if(distance - (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0 <= temp_dist)
				{
					temp_dist = distance - (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
					recule_nulle_obstacle(temp_dist, encA, encB);
					//nxtDisplayTextLine(6, "obstacle_nulle");
					mvt_fait = 1;
				}
				else
				{
					//avance_nulle_obstacle((abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0 + temp_dist, encA, encB);
					recule_nulle_obstacle(temp_dist, encA, encB);
					mvt_fait = 2;
					//nxtDisplayTextLine(5, "obligation d'arret");
				}
				// On est normalement a l'arret.
				motor[motorA] = 0;
				motor[motorB] = 0;
				
				wait1Msec(150);
				distance_faite = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
				return;
			}
			else
			{
				// Detection pour savoir si on a cogne contre un mur, auquel cas on est a l'arret, les roues immobiles.
				if(abs(nMotorEncoder[motorA] - encA_old) < 3 || abs(nMotorEncoder[motorB] - encB_old) < 3)
				{
					compte_arret++;
					// On considere que l'on est a l'arret.
					if(compte_arret >= 70)
					{
						motor[motorA] = 0;
						motor[motorB] = 0;
						mvt_fait = 2;
						return;
					}
				}
				else
					compte_arret = 0;

				dist = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
				
				// Ajoute pour la version avec les tasks.
				distance_faite = dist;
				erreur = d - dist;
				old_puiss = puiss;
				
				if(matrix_or_tetrix == 0)
				{
					puiss =(erreur*0.294-old_erreur*0.290)/denom+offset_motor;
					if(puiss <= puiss_min_lego)
						puiss=puiss_min_lego;
				}
				else if(matrix_or_tetrix == 1)
					puiss =(erreur*1.13-old_erreur*1.12 + (puiss-offset_motor)*1.8)/denom+offset_motor;
				else
					puiss =(erreur*0.43-old_erreur*0.42 + (puiss-offset_motor)*0.37)/denom+offset_motor;
				
				// On met une saturation sur la variation de puissance.
				if(saturation_dpuiss_arriere != 0)
				{
					if(puiss > old_puiss+saturation_dpuiss_arriere)
						puiss = old_puiss+saturation_dpuiss_arriere;
					else if(puiss < old_puiss-saturation_dpuiss_arriere)
						puiss = old_puiss-saturation_dpuiss_arriere;
				}
				
				old_erreur = erreur;
				motor[motorA] = reverse * (int) -puiss;
				motor[motorB] = reverse * (int) -puiss;
				wait1Msec(10);
			}
		}
		motor[motorB]=0;
		motor[motorA]=0;
		mvt_fait = 1;
		for(char i = 0; i < 15; i++)
		{
			wait1Msec(10);
			distance_faite = (abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360 + abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360)/2.0;
		}
}





// -----------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------- Tasks pour les mouvements -------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------------------

task avance_nulleT()
{
	mvt_fait = 2;
	distance_faite = 0;
	avance_nulle_private(distance_a_faire);
	if(mvt_fait == 2)
		mvt_fait = 0;
	else
		mvt_fait = 1;
}

void avance_nulle(float distance, float *distance_parcourue, char *done, char *obstacle_rencontre, int temps = 0, char offset = 100)
{
	char old_offset = offset_motor;
	if(offset != 100)
		offset_motor = offset;
	
	distance_a_faire = distance;
	obstacle_rencontre_private = 0;
	*done = 0;
	
	if(temps == 0)
	{
		// On ne fait rien d'autre qu'avancer. A remarquer qu'aucune information n'est retournee. 
		// On ne donne donc rien sur done ni sur distance_parcourue.
		avance_nulle_private(distance);
		if(mvt_fait == 2)
			mvt_fait = 0;
		else
			mvt_fait = 1;
	}
	else if(temps == -1)
	{
		// Determiner soi-meme le temps a attendre par automatique lineaire.
		/// --------------------------------- A FAIRE ------------------------------------
		startTask(avance_nulleT);
		wait1Msec(temps);
		stopTask(avance_nulleT);
	}
	else
	{
		startTask(avance_nulleT);
		wait1Msec(temps);
		stopTask(avance_nulleT);
	}
	
	motor[motorA] = 0;
	motor[motorB] = 0;
	*done = mvt_fait; // 1 si on l'a fini, 0 sinon.
	*distance_parcourue = distance_faite;
	*obstacle_rencontre=obstacle_rencontre_private;
	offset_motor = old_offset;
}

task recule_nulleT()
{
	mvt_fait = 2;
	distance_faite = 0;
	recule_nulle_private(distance_a_faire);
	if(mvt_fait == 2)
		mvt_fait = 0;
	else
		mvt_fait = 1;
}

void recule_nulle(float distance, float *distance_parcourue, char *done, char *obstacle_rencontre, int temps = 0, char offset = 100)
{
	char old_offset = offset_motor;
	if(offset != 100)
		offset_motor = offset;
	
	distance_a_faire = distance;
	obstacle_rencontre_private = 0;
	*done = 0;
	
	if(temps == 0)
	{
		// On ne fait rien d'autre qu'avancer. A remarquer qu'aucune information n'est retournee. 
		// On ne donne donc rien sur done ni sur distance_parcourue.
		recule_nulle_private(distance);
		if(mvt_fait == 2)
			mvt_fait = 0;
		else
			mvt_fait = 1;
	}
	else if(temps == -1)
	{
		// Determiner soi-meme le temps a attendre par automatique lineaire.
		/// --------------------------------- A FAIRE ------------------------------------
		startTask(recule_nulleT);
		wait1Msec(temps);
		stopTask(recule_nulleT);
	}
	else
	{
		startTask(recule_nulleT);
		wait1Msec(temps);
		stopTask(recule_nulleT);
	}
	
	motor[motorA] = 0;
	motor[motorB] = 0;
	
	*done = mvt_fait; // 1 si on l'a fini, 0 sinon.
	*distance_parcourue = distance_faite;
	*obstacle_rencontre=obstacle_rencontre_private;
	offset_motor = old_offset;
}




// ----------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------- Fonctions de base pour avancer / reculler  -----------------------------------
// ----------------------------------------------------------------------------------------------------------------------------------

float distance_a_faire_anyway;
float distance_parcourue_anyway;
int temps_anyway;
char done_anyway;
char obstacle_rencontre_anyway;
char offset_anyway;

task avance_nulle_anywayT()
{
	avance_nulle(distance_a_faire_anyway-distance_parcourue_anyway, &distance_parcourue_anyway, &done_anyway, &obstacle_rencontre_anyway, temps_anyway, offset_anyway);
	wait1Msec(10);
	while(obstacle_rencontre_anyway == 1 && done_anyway == 0)
	{
		avance_nulle(distance_a_faire_anyway-distance_parcourue_anyway, &distance_parcourue_anyway, &done_anyway, &obstacle_rencontre_anyway, temps_anyway, offset_anyway);
		wait1Msec(10);
	}
}

void avance_nulle_anyway(float distance, float *distance_parcourue, char *done, char *obstacle_rencontre, int temps = 0, char offset = 100)
{
	// fonction qui fait avancer le robot de la distance donnee. 
	// Elle sert juste a gerer les obstacles de maniere plus simple. C'est a dire que si un obstacle est rencontre, elle attend et retente de repartir.
	// Pas de gestion des impossibilites de deplacement.
	distance_a_faire_anyway = distance;
	offset_anyway = offset;
	distance_parcourue_anyway = 0.0;
	if(temps == 0)
	{
		float distance_parcourue_temp = distance_parcourue_anyway;
		do
		{
			distance_parcourue_temp = distance_parcourue_anyway;
			avance_nulle(distance_a_faire_anyway-distance_parcourue_temp, &distance_parcourue_anyway, &done_anyway, &obstacle_rencontre_anyway, 0, offset_anyway);
			wait1Msec(10);
		}while(obstacle_rencontre_anyway == 1 && done_anyway == 0);
	}
	else
	{
		temps_anyway = temps-100;
		startTask(avance_nulle_anywayT);
		wait1Msec(temps);
		stopTask(avance_nulle_anywayT);
	}
	*distance_parcourue=distance_parcourue_anyway;
	*done = done_anyway;
	*obstacle_rencontre = obstacle_rencontre_anyway;
}


task recule_nulle_anywayT()
{
	do
	{
		recule_nulle(distance_a_faire_anyway-distance_parcourue_anyway, &distance_parcourue_anyway, &done_anyway, &obstacle_rencontre_anyway, temps_anyway, offset_anyway);
		wait1Msec(10);
	}while(obstacle_rencontre_anyway == 1 && done_anyway == 0);
}

void recule_nulle_anyway(float distance, float *distance_parcourue, char *done, char *obstacle_rencontre, int temps = 0, char offset = 100)
{
	// fonction qui fait avancer le robot de la distance donnee. 
	// Elle sert juste a gerer les obstacles de maniere plus simple. C'est a dire que si un obstacle est rencontre, elle attend et retente de repartir.
	// Pas de gestion des impossibilites de deplacement.
	distance_a_faire_anyway = distance;
	offset_anyway = offset;
	distance_parcourue_anyway = 0.0;
	if(temps == 0)
	{
		do
		{
			recule_nulle(distance_a_faire_anyway-distance_parcourue_anyway, &distance_parcourue_anyway, &done_anyway, &obstacle_rencontre_anyway, 0, offset_anyway);
			wait1Msec(10);
		}while(obstacle_rencontre_anyway == 1 && done_anyway == 0);
	}
	else
	{
		temps_anyway = temps-100;
		startTask(recule_nulle_anywayT);
		wait1Msec(temps);
		stopTask(recule_nulle_anywayT);
	}
	*distance_parcourue=distance_parcourue_anyway;
	*done = done_anyway;
	*obstacle_rencontre = obstacle_rencontre_anyway;
}

float distance_a_faire_fairplay;
float distance_parcourue_fairplay;
float distance_parcourue_fairplay2;
float distance_parcourue_fp;
float distance_fairplay = 100.0;
int temps_fairplay;
char done_fairplay;
char obstacle_rencontre_fairplay;
char offset_fairplay;


task avance_nulle_fairplayT()
{
	distance_parcourue_fairplay = 0.0;
	distance_a_faire_fairplay = 0.0;
	
	avance_nulle(distance_a_faire_fairplay, &distance_parcourue_fairplay, &done_fairplay, &obstacle_rencontre_fairplay, temps_fairplay, offset_fairplay);
	wait1Msec(100);
	if(done_fairplay == 0 && obstacle_rencontre_fairplay == 1 && distance_parcourue_fairplay > distance_fairplay)
	{
		recule_nulle(distance_fairplay, &distance_parcourue_fp, &done_fairplay, &obstacle_rencontre_fairplay, temps_fairplay, offset_fairplay);
		wait1Msec(2000);
		avance_nulle_anyway(distance_a_faire_fairplay-distance_parcourue_fairplay+distance_parcourue_fp, &distance_parcourue_fairplay2, &done_fairplay, &obstacle_rencontre_fairplay, temps_fairplay, offset_fairplay);
	}
}

void avance_nulle_fairplay(float distance, float *distance_parcourue, char *done, char *obstacle_rencontre, int temps = 0, char offset = 100)
{
	// fonction qui fait avancer le robot de la distance donnee. 
	// Elle sert juste a gerer les obstacles de maniere plus simple. C'est a dire que si un obstacle est rencontre, elle attend et retente de repartir.
	// Pas de gestion des impossibilites de deplacement.
	distance_a_faire_fairplay = distance;
	offset_fairplay = offset;
	distance_parcourue_fairplay2 = 0.0;
	distance_parcourue_fp = 0.0;
	if(temps <= 2500)
	{
		avance_nulle(distance_a_faire_fairplay, &distance_parcourue_fairplay, &done_fairplay, &obstacle_rencontre_fairplay, 0, offset_fairplay);
		wait1Msec(100);
		if(done_fairplay == 0 && obstacle_rencontre_fairplay == 1 && distance_parcourue_fairplay > distance_fairplay)
		{
			recule_nulle(distance_fairplay, &distance_parcourue_fp, &done_fairplay, &obstacle_rencontre_fairplay, 0, offset_fairplay);
			wait1Msec(2000);
			avance_nulle_anyway(distance_a_faire_fairplay-distance_parcourue_fairplay+distance_parcourue_fp, &distance_parcourue_fairplay2, &done_fairplay, &obstacle_rencontre_fairplay, 0, offset_fairplay);
		}
	}
	else
	{
		temps_fairplay = (float)temps/3.0-2100;
		startTask(avance_nulle_fairplayT);
		wait1Msec(temps);
		stopTask(avance_nulle_fairplayT);
	}
	*distance_parcourue=distance_parcourue_fairplay-distance_parcourue_fp+distance_parcourue_fairplay2;
	*done = done_fairplay;
	*obstacle_rencontre = obstacle_rencontre_fairplay;
}



task recule_nulle_fairplayT()
{
	distance_parcourue_fairplay = 0.0;
	distance_a_faire_fairplay = 0.0;
	
	recule_nulle(distance_a_faire_fairplay, &distance_parcourue_fairplay, &done_fairplay, &obstacle_rencontre_fairplay, temps_fairplay, offset_fairplay);
	wait1Msec(100);
	if(done_fairplay == 0 && obstacle_rencontre_fairplay == 1 && distance_parcourue_fairplay > distance_fairplay)
	{
		avance_nulle(distance_fairplay, &distance_parcourue_fp, &done_fairplay, &obstacle_rencontre_fairplay, temps_fairplay, offset_fairplay);
		wait1Msec(2000);
		recule_nulle_anyway(distance_a_faire_fairplay-distance_parcourue_fairplay+distance_parcourue_fp, &distance_parcourue_fairplay2, &done_fairplay, &obstacle_rencontre_fairplay, temps_fairplay, offset_fairplay);
	}
}

void recule_nulle_fairplay(float distance, float *distance_parcourue, char *done, char *obstacle_rencontre, int temps = 0, char offset = 100)
{
	// fonction qui fait avancer le robot de la distance donnee. 
	// Elle sert juste a gerer les obstacles de maniere plus simple. C'est a dire que si un obstacle est rencontre, elle attend et retente de repartir.
	// Pas de gestion des impossibilites de deplacement.
	distance_a_faire_fairplay = distance;
	offset_fairplay = offset;
	distance_parcourue_fairplay2 = 0.0;
	distance_parcourue_fp = 0.0;
	if(temps <= 2500)
	{
		recule_nulle(distance_a_faire_fairplay, &distance_parcourue_fairplay, &done_fairplay, &obstacle_rencontre_fairplay, 0, offset_fairplay);
		wait1Msec(100);
		if(done_fairplay == 0 && obstacle_rencontre_fairplay == 1 && distance_parcourue_fairplay > distance_fairplay)
		{
			avance_nulle(distance_fairplay, &distance_parcourue_fp, &done_fairplay, &obstacle_rencontre_fairplay, 0, offset_fairplay);
			wait1Msec(2000);
			recule_nulle_anyway(distance_a_faire_fairplay-distance_parcourue_fairplay+distance_parcourue_fp, &distance_parcourue_fairplay2, &done_fairplay, &obstacle_rencontre_fairplay, 0, offset_fairplay);
		}
	}
	else
	{
		temps_fairplay = (float)temps/3.0-2100;
		startTask(recule_nulle_fairplayT);
		wait1Msec(temps);
		stopTask(recule_nulle_fairplayT);
	}
	*distance_parcourue=distance_parcourue_fairplay-distance_parcourue_fp+distance_parcourue_fairplay2;
	*done = done_fairplay;
	*obstacle_rencontre = obstacle_rencontre_fairplay;
}

// ----------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------- Fonctions pour tourner -------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------------------


void turnDeg_private(int degree, char droit)
{
	// droit = 0 ; on tourne a gauche
	// droit = 1 ; on tourne a droite
  //reinitialisation des encodeurs
  //nMotorEncoder[motorA] = 0;
  //nMotorEncoder[motorB] = 0;

	//prise de la valeur initial des encoders
	int encA = nMotorEncoder[motorA];
	int encB = nMotorEncoder[motorB];

	int encA_old = encA;
	int encB_old = encB;

	char compte_arretA = 0;
	char compte_arretB = 0;
	
	float puissA = 0;
	float puissB = 0;
	float old_puissA = 0.0;
	float old_puissB = 0.0;
	float erreurA = 0;
	float erreurB = 0;
	float old_erreurA = 0;
	float old_erreurB = 0;

  //calcul de "tickgoal"
  float alpha=(entraxe/(2*rayon));			//alpha theorique
  // int alpha=expe									//alpha experimental ?? determiner avec la batterie enti?��rement charg??
  int tickGoal = alpha*degree;

  // Asservissement des roues.
  float denom = divise_rotation;
  
  float rot = 0.0;

  //comme les deux moteurs riquent d'aller ?? des vitesses l??g?��rement differente
  //on teste les deux encodeurs separement.
  while(abs(nMotorEncoder[motorA]-encA) < tickGoal || abs(nMotorEncoder[motorB]-encB) < tickGoal)
  {
	
	// Detecte si les moteurs sont bloques.
	if(abs(nMotorEncoder[motorA]-encA_old) <= 3)
		compte_arretA++;
	else
		compte_arretA = 0;
	if(abs(nMotorEncoder[motorB]-encB_old) <= 3)
		compte_arretB++;
	else
		compte_arretB = 0;

	if(compte_arretA >= 50 || compte_arretB >= 50)
	{
		motor[motorA] = 0;
		motor[motorB] = 0;
		mvt_fait = 2;
		return;
	}

	// Ajoute pour la version avec les tasks.
	rot = (float)(abs(nMotorEncoder[motorA]-encA)+abs(nMotorEncoder[motorB]-encB))/2.0/alpha;
	rotation_faite = rot;
	old_puissA = puissA;
	old_puissB = puissB;
	
	erreurA = tickGoal-abs(nMotorEncoder[motorA]-encA)*(float)(PI*rayon)/(float)360;
	erreurB = tickGoal-abs(nMotorEncoder[motorB]-encB)*(float)(PI*rayon)/(float)360;
	

	
	if(matrix_or_tetrix == 0)
	{
		puissA =(erreurA*0.294-old_erreurA*0.290)/denom+offset_motor;
		puissB =(erreurB*0.294-old_erreurB*0.290)/denom+offset_motor;
		if(puissA <= puiss_min_lego)
			puissA=puiss_min_lego;
		if(puissB <= puiss_min_lego)
			puissB=puiss_min_lego;
	}
	else if(matrix_or_tetrix == 1)
	{
		puissA =(erreurA*1.13-old_erreurA*1.12 + (puissA-offset_motor)*1.8)/denom+offset_motor;
		puissB =(erreurB*1.13-old_erreurB*1.12 + (puissB-offset_motor)*1.8)/denom+offset_motor;
	}
	else
	{
		puissA =(erreurA*0.43-old_erreurA*0.42 + (puissA-offset_motor)*0.37)/denom+offset_motor;
		puissB =(erreurB*0.43-old_erreurB*0.42 + (puissB-offset_motor)*0.37)/denom+offset_motor;
	}
	
	if(saturation_dpuiss_rotation != 0)
	{
		if(puissA > old_puissA+saturation_dpuiss_rotation)
			puissA = old_puissA+saturation_dpuiss_rotation;
		else if(puissA < old_puissA-saturation_dpuiss_rotation)
			puissA = old_puissA-saturation_dpuiss_rotation;
		
		if(puissB > old_puissB+saturation_dpuiss_rotation)
			puissB = old_puissB+saturation_dpuiss_rotation;
		else if(puissA < old_puissB-saturation_dpuiss_rotation)
			puissB = old_puissB-saturation_dpuiss_rotation;
	}
	
	//On teste chaque encoder pour savoir quand ils atteingnent 'tickGoal'
    if(abs(nMotorEncoder[motorA]-encA) >= tickGoal) {puissA = 0;}
    if(abs(nMotorEncoder[motorB]-encB) >= tickGoal) {puissB = 0;}

	
	encA_old = nMotorEncoder[motorA];
	encB_old = nMotorEncoder[motorB];
	
	//Start the motors in a left point turn.
	if(droit == 0)
	{
		motor[motorA] = reverse * puissA;
		motor[motorB] = -puissB * reverse;
	}
	else
	{
		motor[motorA] = -puissA * reverse;
		motor[motorB] = puissB * reverse;
	}
	wait1Msec(10);
	}
	motor[motorA] = 0;
	motor[motorB] = 0;
	mvt_fait = 1;
	for(char i = 0; i < 10; i++)
	{
		wait1Msec(10);
		rot = (float)(abs(nMotorEncoder[motorA]-encA)+abs(nMotorEncoder[motorB]-encB))/2.0/alpha;
	}
}

task turnRightDegT()
{
	mvt_fait = 2;
	rotation_faite = 0;
	turnDeg_private(rotation_a_faire, 1);
	if(mvt_fait == 2)
		mvt_fait = 0;
	else
		mvt_fait = 1;
}

void turnRightDeg(float angle, float *rotation_parcourue, char *done, int temps = 0, char offset = 100)
{
	char old_offset = offset_motor;
	if(offset != 100)
		offset_motor = offset;
	
	rotation_a_faire = angle;
	*done = 0;
	
	if(temps == 0)
	{
		// On ne fait rien d'autre que tourner. A remarquer qu'aucune information n'est retournee. 
		// On ne donne donc rien sur done ni sur distance_parcourue.
		turnDeg_private(angle, 1);
		if(mvt_fait == 2)
			mvt_fait = 0;
		else
			mvt_fait = 1;
	}
	else if(temps == -1)
	{
		// Determiner soi-meme le temps a attendre par automatique lineaire.
		/// --------------------------------- A FAIRE ------------------------------------
		startTask(turnRightDegT);
		wait1Msec(temps);
		stopTask(turnRightDegT);
	}
	else
	{
		startTask(turnRightDegT);
		wait1Msec(temps);
		stopTask(turnRightDegT);
	}
	
	motor[motorA] = 0;
	motor[motorB] = 0;
	
	*done = mvt_fait; // 1 si on l'a fini, 0 sinon.
	*rotation_parcourue = rotation_faite;
	offset_motor = old_offset;
}

task turnLeftDegT()
{
	mvt_fait = 2;
	rotation_faite = 0;
	turnDeg_private(rotation_a_faire, 0);
	if(mvt_fait == 2)
		mvt_fait = 0;
	else
		mvt_fait = 1;
}

void turnLeftDeg(float angle, float *rotation_parcourue, char *done, int temps = 0, char offset = 100)
{
	char old_offset = offset_motor;
	if(offset != 100)
		offset_motor = offset;
	
	rotation_a_faire = angle;
	*done = 0;
	
	if(temps == 0)
	{
		// On ne fait rien d'autre que tourner. A remarquer qu'aucune information n'est retournee. 
		// On ne donne donc rien sur done ni sur distance_parcourue.
		turnDeg_private(angle, 0);
		if(mvt_fait == 2)
			mvt_fait = 0;
		else
			mvt_fait = 1;
	}
	else if(temps == -1)
	{
		// Determiner soi-meme le temps a attendre par automatique lineaire.
		/// --------------------------------- A FAIRE ------------------------------------
		startTask(turnLeftDegT);
		wait1Msec(temps);
		stopTask(turnLeftDegT);
	}
	else
	{
		startTask(turnLeftDegT);
		wait1Msec(temps);
		stopTask(turnLeftDegT);
	}
	
	motor[motorA] = 0;
	motor[motorB] = 0;
	
	*done = mvt_fait; // 1 si on l'a fini, 0 sinon.
	*rotation_parcourue = rotation_faite;
	offset_motor = old_offset;
}

/*
// Code de test
	//turnLeftDeg(90*2, 0);
	wait1Msec(100);
	avance_nulle(100);
	wait1Msec(100);
	turnLeftDeg(90*2, 0);
	wait1Msec(100);
	avance_nulle(100);
	wait1Msec(100);
	turnLeftDeg(90*2, 0);
	wait1Msec(400);
	turnRightDeg(90*2, 0);
	wait1Msec(100);
	avance_nulle(100);
	wait1Msec(100);
	turnRightDeg(90*2, 0);
	wait1Msec(100);
	avance_nulle(100);
	wait1Msec(100);
	turnRightDeg(90*2, 0);
	wait1Msec(100);
*/
