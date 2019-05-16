/* 
 * File:   1-CLIENT_UDP_UNICAST.cpp
 * Author: Administrateur
 *
 * Created on 14 mars 2013, 14:34
 */


#include <winsock2.h>
#include <cstdlib>
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <winsock.h>


using namespace std;

/*
 * 
 */
//#define DEBUG_SOCKET
//------------------------------------------------------------------------------

void Init_DLL() {
    WSADATA L_Info_DLL;
    if (WSAStartup(MAKEWORD(2, 0), &L_Info_DLL) != 0) exit;


#ifdef DEBUG_SOCKET 
    cout << L_Info_DLL.szDescription << endl;
#endif 
}
//------------------------------------------------------------------------------

void Fin_DLL() {
    int L_OK = WSACleanup();

#ifdef DEBUG_SOCKET
    if (L_OK == 0) cout << "WSACleanup   :  Ok" << endl;
    else cout << "WSACleanup :  ERREUR" << endl;
#endif

}
//------------------------------------------------------------------------------

void Affiche_Erreur() {
    DWORD L_Code_Erreur = GetLastError();

    char L_Message[512];

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, L_Code_Erreur,
            0, L_Message, 511, NULL);
    cerr << " -Erreur [" << L_Code_Erreur << "]   :  " << L_Message << endl;

}

//------------------------------------------------------------------------------
//VARIABLE GLOBALE

SOCKET H_Socket;
sockaddr_in Process_Distant;
//------------------------------------------------------------------------------

void Creation_Socket() {
    H_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (H_Socket == INVALID_SOCKET) {
        Affiche_Erreur();
    }
#ifdef DEBUG_SOCKET
    cout << "Valeur du socket:   " << H_Socket << endl;
#endif
}
//------------------------------------------------------------------------------

void Creation_Info_Adressage(const char* P_nom) {
    
//    hostent* Info_addr;
//    Info_addr = gethostbyname(P_nom);
//    
//    servent* Info_Service;
//    Info_Service = getservbyname("TP_IRIS","TCP");
//    
//    cout<<Info_addr->h_name<<endl;
//    cout<<Info_Service->s_port<<endl;
//    
//    Process_Distant.sin_family = Info_addr->h_addrtype;
//    Process_Distant.sin_port = Info_Service->s_port;
//    Process_Distant.sin_addr.S_un.S_addr = *(u_long *)Info_addr->h_addr_list[0];
    
    Process_Distant.sin_family = AF_INET;
    Process_Distant.sin_port = htons(9999);
    Process_Distant.sin_addr.S_un.S_addr = inet_addr(P_nom);  
    
    
}
//------------------------------------------------------------------------------

void Emission(const void* P_Data, int P_Taille) {
    int L_Taille = send(H_Socket, (char*) P_Data, P_Taille, 0);

    if (L_Taille == SOCKET_ERROR) Affiche_Erreur();

#ifdef DEBUG_SOCKET
    cout << "sendto - Taille  : " << L_Taille << endl;
#endif

}

//------------------------------------------------------------------------------

void Connexion() {

    int L_OK = connect(H_Socket, (sockaddr*) & Process_Distant, sizeof (Process_Distant));

    if (L_OK != 0) Affiche_Erreur();

}

//------------------------------------------------------------------------------

void Deconnexion() {

    int L_OK = shutdown(H_Socket, SD_BOTH);

    closesocket(H_Socket);

}
//------------------------------------------------------------------------------



//int Reception() {
//
//
//    char L_Data[11];
//    int L_Taille = recv(H_Socket, (char*) L_Data, sizeof (L_Data) - 1, 0);
//
//    if (L_Taille == SOCKET_ERROR) Affiche_Erreur();
//    else {
//        L_Data[L_Taille] = 0;
//        cout << L_Data << endl;
//    }
//
//    return L_Taille;
//}

void Reception_Asynchrone(){
    
#define TAILLE_MAX 1000
    
    char L_Data[TAILLE_MAX+1];
    
    WSAEVENT L_Evenement;
    
    WSANETWORKEVENTS L_resultat_evenement;
    
    L_Evenement = WSACreateEvent();
    
    WSAEventSelect(H_Socket, L_Evenement, FD_READ | FD_CLOSE );
    
    bool L_Fin=false;
    int L_Taille;
    DWORD L_raison;
    do{
        
    L_raison = WaitForSingleObject(L_Evenement, 1000);
    
    if (L_raison != 0) {
    
    WSAEnumNetworkEvents(H_Socket, L_Evenement, &L_resultat_evenement);
    
 if ((L_resultat_evenement.lNetworkEvents & FD_READ) == FD_READ ) {
     cout<<"FD_READ : "<<endl;       
    do{
            L_Taille = recv(H_Socket, L_Data, TAILLE_MAX, 0);
            
            if(L_Taille > 0){
                L_Data[L_Taille] = 0;
                cout << L_Data;
            }
            
         }while(L_Taille > 0);
     
} 
    if((L_resultat_evenement.lNetworkEvents & FD_CLOSE)== FD_CLOSE ){
        
        cout<<endl<<"FD_CLOSE !"<<endl;
        L_Fin = true;
    }
    
    }else{
        cout<<"TIME OUT";
    }
    
    }while(L_Fin == false);
}



//------------------------------------------------------------------------------

int main(int argc, char** argv) {

    Init_DLL();

    char L_nom[]="127.0.0.1";
    
    Creation_Socket();
    
    Creation_Info_Adressage(L_nom);

    Connexion();

    const char* L_requete = "Salut !";
    Emission(L_requete, strlen(L_requete));
    
    
    Reception_Asynchrone();

    Deconnexion();

    Fin_DLL();

    cout << endl << endl;
    return 0;
}

