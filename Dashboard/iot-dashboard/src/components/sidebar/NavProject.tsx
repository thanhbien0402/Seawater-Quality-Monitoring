"use client"

import {
    View,
    Upload,
    MoreHorizontal,
    Microchip,
    Trash2,
    type LucideIcon,
} from "lucide-react"

import {
    DropdownMenu,
    DropdownMenuContent,
    DropdownMenuItem,
    DropdownMenuSeparator,
    DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu"
import {
    SidebarGroup,
    SidebarGroupLabel,
    SidebarMenu,
    SidebarMenuAction,
    SidebarMenuButton,
    SidebarMenuItem,
    useSidebar,
} from "@/components/ui/sidebar"
import { useContext, useState } from "react"
import { SelectedDeviceContext } from "../context/SelectedDeviceProvider"
import { toast } from "@/hooks/use-toast"
import { deleteDevice } from "@/actions"
import { AlertDialog, AlertDialogAction, AlertDialogCancel, AlertDialogContent, AlertDialogDescription, AlertDialogFooter, AlertDialogHeader, AlertDialogTitle } from "../ui/alert-dialog"

export function NavProjects() {
    const { isMobile } = useSidebar()

    const { deviceList, setSelectedDevice, refreshDeviceList } = useContext(SelectedDeviceContext)

    const [isDelete, setIsDelete] = useState<boolean>(false)
    const [deleteId, setDeleteId] = useState<string>('')


    const token = global?.window?.localStorage.getItem('token') ?? ''


    return (
        <>
            <SidebarGroup className="group-data-[collapsible=icon]:hidden">
                <SidebarGroupLabel>Device List</SidebarGroupLabel>
                <SidebarMenu>
                    {deviceList.map((item) => (
                        <SidebarMenuItem key={item.id.id}>
                            <SidebarMenuButton asChild>
                                <div className="flex justify-start" onClick={() => setSelectedDevice(item)}>
                                    <Microchip />
                                    <span>{item.name}</span>
                                </div>
                            </SidebarMenuButton>
                            <DropdownMenu>
                                <DropdownMenuTrigger asChild>
                                    <SidebarMenuAction showOnHover>
                                        <MoreHorizontal />
                                        <span className="sr-only">More</span>
                                    </SidebarMenuAction>
                                </DropdownMenuTrigger>
                                <DropdownMenuContent
                                    className="w-48 rounded-lg"
                                    side={isMobile ? "bottom" : "right"}
                                    align={isMobile ? "end" : "start"}
                                >
                                    <DropdownMenuItem className="hover:!text-green-500">
                                        <View className="text-current" />
                                        <span>View Device</span>
                                    </DropdownMenuItem>
                                    <DropdownMenuItem className="hover:!text-yellow-600">
                                        <Upload className="text-current" />
                                        <span>Update Device</span>
                                    </DropdownMenuItem>
                                    <DropdownMenuSeparator />
                                    <DropdownMenuItem className="hover:!text-red-600" onClick={() => {
                                        setIsDelete(true)
                                        setDeleteId(item?.id.id)
                                    }}>
                                        <Trash2 className="text-current" />
                                        <span>Delete Device</span>
                                    </DropdownMenuItem>
                                </DropdownMenuContent>
                            </DropdownMenu>
                        </SidebarMenuItem>
                    ))}
                </SidebarMenu>
            </SidebarGroup>
            <AlertDialog open={isDelete} onOpenChange={setIsDelete}>
                <AlertDialogContent>
                    <AlertDialogHeader>
                        <AlertDialogTitle>Are you absolutely sure?</AlertDialogTitle>
                        <AlertDialogDescription>
                            This action cannot be undone. This will permanently delete your
                            device and remove your data from our servers.
                        </AlertDialogDescription>
                    </AlertDialogHeader>
                    <AlertDialogFooter>
                        <AlertDialogCancel>Cancel</AlertDialogCancel>
                        <AlertDialogAction onClick={async () => {
                            await deleteDevice(deleteId, token)
                            refreshDeviceList()
                            setDeleteId('')
                            toast({
                                title: "You submitted the following values:",
                                description: (
                                    <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
                                        <p className="text-white">Deleted Successfully</p>
                                    </pre>
                                ),
                            })
                        }}>Continue</AlertDialogAction>
                    </AlertDialogFooter>
                </AlertDialogContent>
            </AlertDialog>
        </>
    )
}
