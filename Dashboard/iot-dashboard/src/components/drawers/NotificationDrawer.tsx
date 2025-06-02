import {
    Drawer,
    DrawerClose,
    DrawerContent,
    DrawerDescription,
    DrawerFooter,
    DrawerHeader,
    DrawerTitle,
    DrawerTrigger
} from '../ui/drawer'
import { Button } from '../ui/button'
import { BadgeAlert } from 'lucide-react'
import NotificationTable from '@/app/notifications/page'
import { ScrollArea } from "@/components/ui/scroll-area"


export function NotificationDrawer() {
    return (
        <Drawer>
            <DrawerTrigger asChild>
                <Button variant="ghost">
                    <p>Notification Center</p>
                    <BadgeAlert className="h-4 w-4" />
                </Button>
            </DrawerTrigger>
            <DrawerContent>
                <DrawerHeader className="text-left">
                    <DrawerTitle>Notification Center</DrawerTitle>
                    <DrawerDescription>
                        The notification list of all device
                    </DrawerDescription>
                </DrawerHeader>
                <ScrollArea className="overflow-x-scroll h-[60vh] w-full rounded-md border p-4">
                    <NotificationTable />
                </ScrollArea>
                <DrawerFooter className="pt-2">
                    <DrawerClose asChild>
                        <Button variant="default">Close</Button>
                    </DrawerClose>
                </DrawerFooter>
            </DrawerContent>
        </Drawer>
    )
}
